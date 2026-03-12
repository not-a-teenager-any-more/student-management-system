#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置全局样式，类似 CSS
    a.setStyleSheet(R"(
        QWidget { font-size: 14px; font-family: 'Microsoft YaHei'; }
        QPushButton {
            background-color: #0078d7;
            color: white;
            border-radius: 4px;
            padding: 5px 15px;
        }
        QPushButton:hover { background-color: #1084e3; }
        QLineEdit {
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 4px;
        }
        QListWidget {
            background-color: #f0f0f0;
            border: none;
        }
        QListWidget::item {
            height: 40px;
            padding-left: 10px;
        }
        QListWidget::item:selected {
            background-color: #white;
            color: #0078d7;
            border-left: 4px solid #0078d7;
        }
    )");

    MainWindow w;
    w.show();
    return a.exec();
}
