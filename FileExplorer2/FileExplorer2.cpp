#include "FileExplorer2.h"
#include"Algorithms.h"
#include <future>
#include"Directory.h"
#include <iostream>
#include<memory>

#include "Definitions.h"
#include "File.h"

consteval auto sum(auto integer_one, auto integer_two)
{
    return integer_one + integer_two;
}

auto getSize(const FileSystemNode* node)
{
    return node->get_size();
}

void test(std::vector<int>& ref)
{
	ref.push_back(99);
}

FileExplorer2::FileExplorer2(QWidget* parent)
	: QMainWindow(parent)
{

	TIMER 
	Directory directory1(L"C:\\");

	std::cout << directory1.get_size() << std::endl;

	const std::unique_ptr<std::vector<std::unique_ptr<FileSystemNode>>> nigger = std::make_unique<
		FileSystemNode::FSNodes>();
}
 