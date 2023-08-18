#pragma once
#include<atomic>
#include<iostream>
#include<mutex>
#include <syncstream>
#include "FileSystemNode.h"

class Directory : public FileSystemNode
{
private:
    mutable std::atomic<uint64_t> m_file_counter_{};
	mutable	std::atomic<uint64_t> m_folder_counter_{};
	mutable std::mutex m_contents_vector_mutex;
	mutable FSNodes m_contents_;
	static void m_get_all_in_directory(std::stop_token stop_token, std::mutex& p_vec_mutex, FSNodes& nodes,const std::filesystem::path& p_dir_path);
public:
	Directory() = delete;
	template<typename T, typename = std::enable_if<std::is_convertible_v<T, std::filesystem::path>>>
	explicit Directory(T&& path) : FileSystemNode(std::forward<T>(path)) {}
	Directory(const Directory& ref);
	Directory& operator=(const Directory& directory)
	{
		this->m_path_ = directory.m_path_;
		this->m_name_ = directory.m_name_;
		this->m_path_ = directory.m_path_;
		this->m_last_access_date_ = directory.m_last_access_date_;
		this->m_last_modification_date_ = directory.m_last_modification_date_;
		this->m_last_modification_date_ = directory.m_last_modification_date_;
		this->m_file_counter_.store(directory.m_file_counter_.load());
		this->m_folder_counter_.store(directory.m_folder_counter_.load());
		return *this;
	}
	Directory directory(const Directory& directory)
	{
		this->m_path_ = directory.m_path_;
		this->m_name_ = directory.m_name_;
		this->m_path_ = directory.m_path_;
		this->m_last_access_date_ = directory.m_last_access_date_;
		this->m_last_modification_date_ = directory.m_last_modification_date_;
		this->m_last_modification_date_ = directory.m_last_modification_date_;
		this->m_file_counter_.store(directory.m_file_counter_.load());
		this->m_folder_counter_.store(directory.m_folder_counter_.load());
	}
	static Directory get_copy(const Directory& dir)
	{
		Directory dir1 = dir;

		return dir;
	}
	[[nodiscard]] static uint64_t s_get_size(const std::filesystem::path p_path);
	[[nodiscard]] uint64_t get_size() const override;
	[[nodiscard]] std::filesystem::path get_parent_directory() const;
	[[nodiscard]] const FSNodes& get_all_contents() const;
	void get_all_in_directory(const std::filesystem::path& path) const;
	//static void s_get_all_in_directory(std::stop_token stop_token, std::optional<std::vector<std::unique_ptr<FileSystemNode>>>& res, Directory& directory);s
	static void s_get_all_in_directory(std::stop_token token, std::optional<std::vector<std::unique_ptr<FileSystemNode>>>& vector,const std::filesystem::path& path);
};
