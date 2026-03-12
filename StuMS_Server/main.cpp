// main.cpp
#include "serverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("Student Management Server");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("YourCompany");
    
    ServerWindow w;
    w.show();
    
    return a.exec();
}