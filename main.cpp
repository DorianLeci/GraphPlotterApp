#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow glavni_prozor;
    glavni_prozor.show();
    glavni_prozor.resize(2560,1440);
    return a.exec();
}
