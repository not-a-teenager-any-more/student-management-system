/********************************************************************************
** Form generated from reading UI file 'serverwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWINDOW_H
#define UI_SERVERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *portSpinBox;
    QPushButton *startButton;
    QPushButton *clearLogButton;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *logTextEdit;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QTextEdit *testRequestEdit;
    QPushButton *sendTestButton;

    void setupUi(QMainWindow *ServerWindow)
    {
        if (ServerWindow->objectName().isEmpty())
            ServerWindow->setObjectName("ServerWindow");
        ServerWindow->resize(800, 600);
        centralwidget = new QWidget(ServerWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        portSpinBox = new QSpinBox(groupBox);
        portSpinBox->setObjectName("portSpinBox");
        portSpinBox->setMinimum(1024);
        portSpinBox->setMaximum(65535);
        portSpinBox->setValue(12345);

        horizontalLayout->addWidget(portSpinBox);

        startButton = new QPushButton(groupBox);
        startButton->setObjectName("startButton");

        horizontalLayout->addWidget(startButton);

        clearLogButton = new QPushButton(groupBox);
        clearLogButton->setObjectName("clearLogButton");

        horizontalLayout->addWidget(clearLogButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(groupBox);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        logTextEdit = new QTextEdit(tab);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setReadOnly(true);

        verticalLayout_2->addWidget(logTextEdit);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        label_2 = new QLabel(tab_2);
        label_2->setObjectName("label_2");

        verticalLayout_3->addWidget(label_2);

        testRequestEdit = new QTextEdit(tab_2);
        testRequestEdit->setObjectName("testRequestEdit");

        verticalLayout_3->addWidget(testRequestEdit);

        sendTestButton = new QPushButton(tab_2);
        sendTestButton->setObjectName("sendTestButton");

        verticalLayout_3->addWidget(sendTestButton);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        ServerWindow->setCentralWidget(centralwidget);

        retranslateUi(ServerWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ServerWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ServerWindow)
    {
        ServerWindow->setWindowTitle(QCoreApplication::translate("ServerWindow", "\345\255\246\347\224\237\344\277\241\346\201\257\347\256\241\347\220\206\347\263\273\347\273\237 - \346\234\215\345\212\241\345\231\250", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ServerWindow", "\346\234\215\345\212\241\345\231\250\346\216\247\345\210\266", nullptr));
        label->setText(QCoreApplication::translate("ServerWindow", "\347\253\257\345\217\243:", nullptr));
        startButton->setText(QCoreApplication::translate("ServerWindow", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", nullptr));
        clearLogButton->setText(QCoreApplication::translate("ServerWindow", "\346\270\205\351\231\244\346\227\245\345\277\227", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("ServerWindow", "\351\200\232\344\277\241\346\227\245\345\277\227", nullptr));
        label_2->setText(QCoreApplication::translate("ServerWindow", "\346\265\213\350\257\225\350\257\267\346\261\202 (JSON\346\240\274\345\274\217):", nullptr));
        testRequestEdit->setPlainText(QCoreApplication::translate("ServerWindow", "{\n"
"    \"type\": \"login\",\n"
"    \"data\": {\n"
"        \"username\": \"admin\",\n"
"        \"password\": \"123456\",\n"
"        \"role\": \"admin\"\n"
"    }\n"
"}", nullptr));
        sendTestButton->setText(QCoreApplication::translate("ServerWindow", "\345\217\221\351\200\201\346\265\213\350\257\225\350\257\267\346\261\202", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("ServerWindow", "\346\265\213\350\257\225\345\267\245\345\205\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerWindow: public Ui_ServerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWINDOW_H
