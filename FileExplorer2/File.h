#pragma once
#include "FileSystemNode.h"

class File : public FileSystemNode
{
public:
	[[nodiscard]] uint64_t get_size() const override;
};

inline uint64_t File::get_size() const
{
	return std::filesystem::file_size(m_path_);
}
