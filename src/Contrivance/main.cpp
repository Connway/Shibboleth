#include "Contrivance_ContrivanceWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ContrivanceWindow w;
    w.showMaximized();

    return a.exec();
}
