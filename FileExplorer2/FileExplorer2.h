#pragma once

#include <QtWidgets/QMainWindow>
#include "FileSystemNode.h"
#include "ui_FileExplorer2.h"

class FileExplorer2 : public QMainWindow
{
    Q_OBJECT
public:
    explicit FileExplorer2(QWidget *parent = nullptr);
	~FileExplorer2() override = default;
private:
   Ui::FileExplorer2Class ui;
   std::filesystem::path m_current_path_;
   std::mutex vector_mutex_;
   FileSystemNode::FSNodes m_nodes_;
   FileSystemNode::FSNodes m_search_nodes_;
};
