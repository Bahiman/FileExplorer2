#pragma once
#include "FileSystemNode.h"

class File : public FileSystemNode
{
public:
	[[nodiscard]] uint64_t get_size() const override;
	template<typename T, typename = std::enable_if<std::is_convertible_v<T,std::filesystem::path>>>
	explicit File(T&& path) : FileSystemNode(std::forward<T>(path)){}
	File& operator=(const File& file) 
	{
		this->m_path_ = file.m_path_;
		this->m_name_ = file.m_name_;
		this->m_path_ = file.m_path_;
		this->m_last_access_date_ = file.m_last_access_date_;
		this->m_last_modification_date_ = file.m_last_modification_date_;
		this->m_last_modification_date_ = file.m_last_modification_date_;
		return *this;
	}

	static File get_copy(const File& file)
	{
		File file1 = file;

		return file1;
	}
	//
	//File(const File& file) : FileSystemNode(m_path_)
	//{
	//	this->m_name_ = file.m_name_;
	//	this->m_path_ = file.m_path_;
	//	this->m_last_access_date_ = file.m_last_access_date_;
	//	this->m_last_modification_date_ = file.m_last_modification_date_;
	//	this->m_last_modification_date_ = file.m_last_modification_date_;
	//}
};

inline uint64_t File::get_size() const
{
	return std::filesystem::file_size(m_path_);
}

