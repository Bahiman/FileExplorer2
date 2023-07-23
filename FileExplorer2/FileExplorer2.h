#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileExplorer2.h"

class FileExplorer2 : public QMainWindow
{
    Q_OBJECT

public:
    FileExplorer2(QWidget *parent = nullptr);
    ~FileExplorer2();

private:
    Ui::FileExplorer2Class ui;
};
