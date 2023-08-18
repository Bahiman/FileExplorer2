#include "Directory.h"
#include"CancellableThreadPool.h"
#include<QDirIterator>
#include<chrono>
#include <execution>
#include <filesystem>
#include <future>
#include <iostream>
#include<mutex>

#include "APTH.h"
#include "File.h"
#include"ThreadPool.h"

// Copilot generate a timer class that Yan Chernikov used
// please
// https://www.youtube.com/watch?v=2rK6Wf6Jlzw

class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;

public:

	void Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();

		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		auto duration = end - start;

		double ms = duration * 0.001;

		std::cout << duration << "us (" << ms << "ms)\n";
	}

	Timer()
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();

	}
	~Timer()
	{
		Stop();
	}
};

#ifndef _DEBUG
#define TIMER Timer t;
#else
#define TIMER
#endif

void Directory::m_get_all_in_directory(std::stop_token stop_token, std::mutex& p_vec_mutex, FSNodes& nodes,
	const std::filesystem::path& p_dir_path)
{
	auto it = std::filesystem::recursive_directory_iterator(p_dir_path);

	FSNodes local_nodes;

	while(it != std::filesystem::recursive_directory_iterator() && !stop_token.stop_requested())
	{
		if(const auto& entry = *it; entry.is_regular_file() && !entry.is_directory())
		{
			local_nodes.emplace_back(std::make_unique<File>(entry.path()));
		}
		else if(!entry.is_regular_file() && entry.is_directory())
		{
			local_nodes.emplace_back(std::make_unique<Directory>(entry.path()));
		}
	}

	std::lock_guard lock(p_vec_mutex);

	// insert into the fsnodes using the move iterator
	nodes.insert(nodes.end(), std::make_move_iterator(local_nodes.begin()), std::make_move_iterator(local_nodes.end()));
}

Directory::Directory(const Directory& ref) : FileSystemNode(ref.get_path())
{
	m_file_counter_ = ref.m_file_counter_.load();
	m_folder_counter_ = ref.m_folder_counter_.load();
}



uint64_t Directory::s_get_size(const std::filesystem::path p_path)
{
	uint64_t total_size = 0;
	if (!std::filesystem::is_directory(p_path))
		throw std::invalid_argument("Path must be a directory.");

	for (const auto& entry : std::filesystem::recursive_directory_iterator(p_path, std::filesystem::directory_options::skip_permission_denied))
	{
		try
		{
			if (entry.is_regular_file())
			{
				total_size += entry.file_size();
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "Something went wrong in `s_get_size` " << e.what() << std::endl;
		}
	}

	return total_size;
	
}

uint64_t Directory::get_size() const
{

	TIMER

	uint64_t total = 0;

	std::vector<std::future<uint64_t>> directory_sizes;

	ThreadPool thread_pool(APTH::rec_t_count);

	for(const auto& entry : std::filesystem::directory_iterator(m_path_,std::filesystem::directory_options::skip_permission_denied))
	{
		if(entry.is_regular_file() && !entry.is_directory())
		{
			total += entry.file_size();
			// 
			++m_file_counter_;
		}
		else
		{
			++m_folder_counter_;
			directory_sizes.emplace_back(thread_pool.enqueue([](const std::filesystem::path& path)
			{
					return s_get_size(path);
				}, entry.path()
			));
		}
	}

	for(auto& future : directory_sizes)
	{
		total += future.get();
	}

	std::cout << m_folder_counter_ << "||" << m_file_counter_ << std::endl;
	
	return total;
}

std::filesystem::path Directory::get_parent_directory() const
{
	if (!m_path_.has_parent_path())
		return  m_path_;

	return m_path_.parent_path();
}

const FileSystemNode::FSNodes& Directory::get_all_contents() const
{

	try
	{
		TIMER

		ThreadPool thread_pool(std::thread::hardware_concurrency());

		std::vector<std::future<void>> tasks;

		auto iterator = std::filesystem::directory_iterator(m_path_);

		while(iterator != std::filesystem::directory_iterator())
		{
			if (const auto& entry = *iterator; entry.is_regular_file())
					{
						std::unique_lock lock(m_contents_vector_mutex);
						m_contents_.emplace_back(std::make_unique<File>(entry.path()));
					}
					else if (entry.is_directory())
					{
						std::unique_lock lock(m_contents_vector_mutex);
						m_contents_.emplace_back(std::make_unique<Directory>(entry.path()));
						auto& path = entry.path();
						tasks.emplace_back(thread_pool.enqueue([path, this]
						{
							get_all_in_directory(path);
						}));
					}
			++iterator;
		}

		for (auto& task : tasks)
		{
			task.get();
		}
	}
	catch (std::exception& e)
	{
		std::wcerr << e.what() << std::endl;
	}
	
	return m_contents_;
}


void Directory::get_all_in_directory(const std::filesystem::path& path) const
{
	try
	{

		std::vector<std::unique_ptr<FileSystemNode>> elems;

		for (auto& entry : std::filesystem::recursive_directory_iterator(path,std::filesystem::directory_options::skip_permission_denied))
		{
			if (entry.is_regular_file() && !entry.is_directory())
			{
				elems.emplace_back(std::make_unique<File>(entry.path()));
			}
			else
			{
				elems.emplace_back(std::make_unique<Directory>(entry.path()));
			}
		}

		std::unique_lock lock(m_contents_vector_mutex);

		// move the elements from elems to m_contents_vector
		m_contents_.insert(m_contents_.end(), std::make_move_iterator(elems.begin()),
		                   std::make_move_iterator(elems.end()));
	}
	catch (std::exception& e)
	{
		std::wcerr << e.what() << std::endl;
	}
}

void Directory::s_get_all_in_directory(std::stop_token token,
	std::optional<std::vector<std::unique_ptr<FileSystemNode>>>& vector, const std::filesystem::path& path)
{
	if(!std::filesystem::is_directory(path))
		throw std::invalid_argument("Path must be a directory.");

	std::mutex vec_mutex;

	vector.emplace();

	std::vector<std::unique_ptr<FileSystemNode>> elems;

	auto it = std::filesystem::directory_iterator(path);

	ThreadPool thread_pool(3);

	std::vector<std::future<void>> tasks;

	while(it != std::filesystem::directory_iterator() && !token.stop_requested()) 
	{
		if (const auto& entry = *it; entry.is_regular_file() && !entry.is_directory())
		{
			std::lock_guard l11111111111111111111111111ock(vec_mutex);
			elems.emplace_back(std::make_unique<File>(entry.path()));
		}
		else
		{
			std::lock_guard lock(vec_mutex);
			const auto& entry_path = entry.path();
			elems.emplace_back(std::make_unique<Directory>(entry_path));
			tasks.emplace_back(thread_pool.enqueue(&Directory::m_get_all_in_directory,std::stop_token(),std::ref(vec_mutex),std::ref(elems),entry_path));
		}
		++it;
	}

	for(auto& task : tasks)
	{
		task.get();
	}
}

