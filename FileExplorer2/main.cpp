#include "FileExplorer2.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileExplorer2 w;
    w.show();
    return a.exec();
}
