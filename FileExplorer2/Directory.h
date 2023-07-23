#pragma once
#include "FileSystemNode.h"

class Directory : public FileSystemNode
{
public:
	Directory() = delete;
	template<typename T, typename = std::enable_if<std::is_convertible_v<T, std::filesystem::path>>>
	explicit Directory(T&& path) : FileSystemNode(std::forward<T>(path)) {}
	[[nodiscard]] static uint64_t s_get_size(const std::filesystem::path& p_path);
	[[nodiscard]] uint64_t get_size() const override;
	[[nodiscard]] std::filesystem::path get_parent_directory() const;
};
