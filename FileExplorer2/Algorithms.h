#pragma once
#include<memory>
#include<vector>

#include "Directory.h"
#include"FileSystemNode.h"

namespace algorithms
{
	auto find_closest(const FileSystemNode::FSNodes& elems, std::wstring_view word) -> std::optional<std::vector<std::unique_ptr<FileSystemNode>>>;
}