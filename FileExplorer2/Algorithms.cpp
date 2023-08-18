#include "Algorithms.h"
#include <execution>
#include <fstream>
#include <iostream>
#include "Definitions.h"
#include "Directory.h"
#include "File.h"


auto algorithms::find_closest(const FileSystemNode::FSNodes& elems, std::wstring_view word) ->
	std::optional<std::vector<std::unique_ptr<FileSystemNode>>>
{
	try
	{
		TIMER
		std::optional<std::vector<std::unique_ptr<FileSystemNode>>> res;
		auto comparator = [&word](const auto& node) -> bool {
			return node->get_name().wstring().find(word) != std::wstring::npos;
		};
		auto add_to = [&](auto& result, auto& p_arg_ptr) -> void {
		if (const auto* file_pointer = dynamic_cast<File*>(p_arg_ptr.get()))
		{
			result->emplace_back().reset(new File(File::get_copy(*file_pointer)));
		}
		else
		{
			const auto* directory_pointer = dynamic_cast<Directory*>(p_arg_ptr.get());
			result->emplace_back().reset(new Directory(Directory::get_copy(*directory_pointer)));
		}
		};
		if (auto it = std::ranges::find_if(elems, comparator); it != elems.end())
		{
			res.emplace();            
			add_to(res, *it);
			while (it = std::find_if(it, elems.end(), comparator), it != elems.end() )
			{
				add_to(res, *it);
				++it;
			}
			std::fstream stream("log.txt", std::ios::trunc);
			std::ranges::for_each(res.value(), [&stream](auto& node) -> void
			{
					stream << node->get_path() << '\n';
			});
		}
		return res;
	}
	catch (std::exception& e)
	{
		std::wcerr << e.what() << std::endl;
	}
	return std::nullopt;
}
