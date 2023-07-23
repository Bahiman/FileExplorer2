#include "Directory.h"
#include "QFileInfo"
#include <omp.h>
#include<Windows.h>
#include <future>
#include <iostream>
#include <filesystem>
#include<chrono>
#include <qstring.h>
#include"ThreadPoo.h"

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


uint64_t Directory::s_get_size(const std::filesystem::path& p_path)
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
				//std::cout << entry.path() << " " << entry.file_size() << '\n';
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "Something fucked up: " << e.what() << std::endl;
		}
	}

	return total_size;
	
}

uint64_t Directory::get_size() const
{
	Timer timer;
	uint64_t total = 0;

	std::vector<std::future<uint64_t>> directory_sizes;

	ThreadPool thread_pool(std::thread::hardware_concurrency()/3);

	for(const auto& entry : std::filesystem::directory_iterator(m_path_,std::filesystem::directory_options::skip_permission_denied))
	{
		if(entry.is_regular_file() && !entry.is_directory())
		{
			total += entry.file_size();
		}
		else
		{
			directory_sizes.emplace_back(thread_pool.enqueue([](const std::filesystem::path& path)
			{
					return s_get_size(path);
			},			entry.path()
			));

		}
	}

	for(auto& future : directory_sizes)
	{
		total += future.get();
	}

	return total;
}

std::filesystem::path Directory::get_parent_directory() const
{
	if (!m_path_.has_parent_path())
		return  m_path_;

	return m_path_.parent_path();
}
