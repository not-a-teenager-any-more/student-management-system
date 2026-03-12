QT += core gui network sql charts widgets
QT += core gui network widgets
QT       += core gui network widgets multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StudentClient
TEMPLATE = app

SOURCES += \
    aichatdialog.cpp \
    chartwidget.cpp \
    deletebuttondelegate.cpp \
    exporttask.cpp \
    exporttaskmodel.cpp \
    exportwidget.cpp \
    exportworker.cpp \
    infowidget.cpp \
    loginwidget.cpp \
    main.cpp \
    clientwindow.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    registerwidget.cpp \
    studentdialog.cpp \
    studenthomewidget.cpp \
    studentwidget.cpp

HEADERS += \
    aichatdialog.h \
    chartwidget.h \
    clientwindow.h \
    deletebuttondelegate.h \
    exporttask.h \
    exporttaskmodel.h \
    exportwidget.h \
    exportworker.h \
    infowidget.h \
    loginwidget.h \
    mainwindow.h \
    networkmanager.h \
    registerwidget.h \
    studentdialog.h \
    studenthomewidget.h \
    studentwidget.h

FORMS += \
    clientwindow.ui \
    studentdialog.ui

# 启用C++11
CONFIG += c++11

include($$PWD/QXlsx/QXlsx.pri)             # QXlsx源代码，版本V1.4.3  https://github.com/QtExcel/QXlsx/releases
INCLUDEPATH += $$PWD/QXlsx


# msvc >= 2017  编译器使用utf-8编码
msvc {
    greaterThan(QMAKE_MSC_VER, 1900){       # msvc编译器版本大于2015
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
    }else{
#        message(msvc2015及以下版本在代码中使用【pragma execution_character_set("utf-8")】指定编码)
    }
}

RESOURCES += \
    StuMS.qrc
