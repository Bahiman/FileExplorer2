#include "FileExplorer2.h"

#include <iostream>

#include "Test.h"
#include"Directory.h"
#include<memory>

FileExplorer2::FileExplorer2(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	std::unique_ptr<FileSystemNode> root = std::make_unique<Directory>("C:\\Users\\bahge\\OneDrive\\Documents");
	std::unique_ptr<FileSystemNode> root2 = std::make_unique<Directory>("D:\\");
	std::thread thread([&]
	{
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << root->get_size() << std::endl;
	}
	);
	std::thread threadTwo(
		[&]
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(850));
			std::cout << root2->get_size();
		}
		);
	//thread.join();
	//std::cout << root->get_size() << std::endl;
}   

FileExplorer2::~FileExplorer2()
{
	
}
