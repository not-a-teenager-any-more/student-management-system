/********************************************************************************
** Form generated from reading UI file 'clientwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTWINDOW_H
#define UI_CLIENTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *loginGroupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *serverAddressEdit;
    QLabel *label_2;
    QLineEdit *serverPortEdit;
    QLabel *label_3;
    QLineEdit *usernameEdit;
    QLabel *label_4;
    QLineEdit *passwordEdit;
    QLabel *label_5;
    QComboBox *roleComboBox;
    QPushButton *loginButton;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_6;
    QComboBox *themeComboBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QTabWidget *mainTabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *searchEdit;
    QPushButton *searchButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *updateButton;
    QTableView *studentsTableView;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *addToExportBtn;
    QLabel *label_format;
    QComboBox *formatComboBox;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *chartsLayout;
    QWidget *tab_export;
    QVBoxLayout *verticalLayout_export;
    QGroupBox *groupBox_exportPath;
    QHBoxLayout *horizontalLayout_exportPath;
    QLabel *label_exportPath;
    QLineEdit *exportPathEdit;
    QPushButton *browseExportPathBtn;
    QTableView *exportTableView;
    QHBoxLayout *horizontalLayout_exportBtn;
    QSpacerItem *horizontalSpacer_export;
    QPushButton *exportBtn;
    QLabel *statusLabel;

    void setupUi(QMainWindow *ClientWindow)
    {
        if (ClientWindow->objectName().isEmpty())
            ClientWindow->setObjectName("ClientWindow");
        ClientWindow->resize(1000, 700);
        centralwidget = new QWidget(ClientWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        loginGroupBox = new QGroupBox(centralwidget);
        loginGroupBox->setObjectName("loginGroupBox");
        horizontalLayout = new QHBoxLayout(loginGroupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(loginGroupBox);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        serverAddressEdit = new QLineEdit(loginGroupBox);
        serverAddressEdit->setObjectName("serverAddressEdit");

        horizontalLayout->addWidget(serverAddressEdit);

        label_2 = new QLabel(loginGroupBox);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        serverPortEdit = new QLineEdit(loginGroupBox);
        serverPortEdit->setObjectName("serverPortEdit");

        horizontalLayout->addWidget(serverPortEdit);

        label_3 = new QLabel(loginGroupBox);
        label_3->setObjectName("label_3");

        horizontalLayout->addWidget(label_3);

        usernameEdit = new QLineEdit(loginGroupBox);
        usernameEdit->setObjectName("usernameEdit");

        horizontalLayout->addWidget(usernameEdit);

        label_4 = new QLabel(loginGroupBox);
        label_4->setObjectName("label_4");

        horizontalLayout->addWidget(label_4);

        passwordEdit = new QLineEdit(loginGroupBox);
        passwordEdit->setObjectName("passwordEdit");
        passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);

        horizontalLayout->addWidget(passwordEdit);

        label_5 = new QLabel(loginGroupBox);
        label_5->setObjectName("label_5");

        horizontalLayout->addWidget(label_5);

        roleComboBox = new QComboBox(loginGroupBox);
        roleComboBox->addItem(QString());
        roleComboBox->addItem(QString());
        roleComboBox->setObjectName("roleComboBox");

        horizontalLayout->addWidget(roleComboBox);

        loginButton = new QPushButton(loginGroupBox);
        loginButton->setObjectName("loginButton");

        horizontalLayout->addWidget(loginButton);


        verticalLayout->addWidget(loginGroupBox);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        verticalLayout_6 = new QVBoxLayout(groupBox);
        verticalLayout_6->setObjectName("verticalLayout_6");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        horizontalLayout_3->addWidget(label_6);

        themeComboBox = new QComboBox(groupBox);
        themeComboBox->setObjectName("themeComboBox");

        horizontalLayout_3->addWidget(themeComboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_6->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");

        verticalLayout_6->addLayout(horizontalLayout_4);


        verticalLayout->addWidget(groupBox);

        mainTabWidget = new QTabWidget(centralwidget);
        mainTabWidget->setObjectName("mainTabWidget");
        mainTabWidget->setEnabled(false);
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        searchEdit = new QLineEdit(tab);
        searchEdit->setObjectName("searchEdit");

        horizontalLayout_2->addWidget(searchEdit);

        searchButton = new QPushButton(tab);
        searchButton->setObjectName("searchButton");

        horizontalLayout_2->addWidget(searchButton);

        addButton = new QPushButton(tab);
        addButton->setObjectName("addButton");

        horizontalLayout_2->addWidget(addButton);

        deleteButton = new QPushButton(tab);
        deleteButton->setObjectName("deleteButton");

        horizontalLayout_2->addWidget(deleteButton);

        updateButton = new QPushButton(tab);
        updateButton->setObjectName("updateButton");

        horizontalLayout_2->addWidget(updateButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        studentsTableView = new QTableView(tab);
        studentsTableView->setObjectName("studentsTableView");

        verticalLayout_2->addWidget(studentsTableView);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        addToExportBtn = new QPushButton(tab);
        addToExportBtn->setObjectName("addToExportBtn");

        horizontalLayout_5->addWidget(addToExportBtn);

        label_format = new QLabel(tab);
        label_format->setObjectName("label_format");

        horizontalLayout_5->addWidget(label_format);

        formatComboBox = new QComboBox(tab);
        formatComboBox->addItem(QString());
        formatComboBox->addItem(QString());
        formatComboBox->setObjectName("formatComboBox");

        horizontalLayout_5->addWidget(formatComboBox);


        verticalLayout_2->addLayout(horizontalLayout_5);

        mainTabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        chartsLayout = new QHBoxLayout();
        chartsLayout->setObjectName("chartsLayout");

        verticalLayout_3->addLayout(chartsLayout);

        mainTabWidget->addTab(tab_2, QString());
        tab_export = new QWidget();
        tab_export->setObjectName("tab_export");
        verticalLayout_export = new QVBoxLayout(tab_export);
        verticalLayout_export->setObjectName("verticalLayout_export");
        groupBox_exportPath = new QGroupBox(tab_export);
        groupBox_exportPath->setObjectName("groupBox_exportPath");
        horizontalLayout_exportPath = new QHBoxLayout(groupBox_exportPath);
        horizontalLayout_exportPath->setObjectName("horizontalLayout_exportPath");
        label_exportPath = new QLabel(groupBox_exportPath);
        label_exportPath->setObjectName("label_exportPath");

        horizontalLayout_exportPath->addWidget(label_exportPath);

        exportPathEdit = new QLineEdit(groupBox_exportPath);
        exportPathEdit->setObjectName("exportPathEdit");

        horizontalLayout_exportPath->addWidget(exportPathEdit);

        browseExportPathBtn = new QPushButton(groupBox_exportPath);
        browseExportPathBtn->setObjectName("browseExportPathBtn");

        horizontalLayout_exportPath->addWidget(browseExportPathBtn);


        verticalLayout_export->addWidget(groupBox_exportPath);

        exportTableView = new QTableView(tab_export);
        exportTableView->setObjectName("exportTableView");
        exportTableView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        exportTableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        exportTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        exportTableView->horizontalHeader()->setStretchLastSection(true);
        exportTableView->verticalHeader()->setVisible(false);

        verticalLayout_export->addWidget(exportTableView);

        horizontalLayout_exportBtn = new QHBoxLayout();
        horizontalLayout_exportBtn->setObjectName("horizontalLayout_exportBtn");
        horizontalSpacer_export = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_exportBtn->addItem(horizontalSpacer_export);

        exportBtn = new QPushButton(tab_export);
        exportBtn->setObjectName("exportBtn");
        exportBtn->setMinimumSize(QSize(100, 30));

        horizontalLayout_exportBtn->addWidget(exportBtn);


        verticalLayout_export->addLayout(horizontalLayout_exportBtn);

        mainTabWidget->addTab(tab_export, QString());

        verticalLayout->addWidget(mainTabWidget);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");

        verticalLayout->addWidget(statusLabel);

        ClientWindow->setCentralWidget(centralwidget);

        retranslateUi(ClientWindow);

        mainTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ClientWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ClientWindow)
    {
        ClientWindow->setWindowTitle(QCoreApplication::translate("ClientWindow", "\345\255\246\347\224\237\344\277\241\346\201\257\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        loginGroupBox->setTitle(QCoreApplication::translate("ClientWindow", "\347\231\273\345\275\225", nullptr));
        label->setText(QCoreApplication::translate("ClientWindow", "\346\234\215\345\212\241\345\231\250:", nullptr));
        label_2->setText(QCoreApplication::translate("ClientWindow", "\347\253\257\345\217\243:", nullptr));
        label_3->setText(QCoreApplication::translate("ClientWindow", "\347\224\250\346\210\267\345\220\215:", nullptr));
        label_4->setText(QCoreApplication::translate("ClientWindow", "\345\257\206\347\240\201:", nullptr));
        label_5->setText(QCoreApplication::translate("ClientWindow", "\350\247\222\350\211\262:", nullptr));
        roleComboBox->setItemText(0, QCoreApplication::translate("ClientWindow", "\347\256\241\347\220\206\345\221\230", nullptr));
        roleComboBox->setItemText(1, QCoreApplication::translate("ClientWindow", "\345\255\246\347\224\237", nullptr));

        loginButton->setText(QCoreApplication::translate("ClientWindow", "\347\231\273\345\275\225", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ClientWindow", "\347\225\214\351\235\242\350\256\276\347\275\256", nullptr));
        label_6->setText(QCoreApplication::translate("ClientWindow", "\344\270\273\351\242\230:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("ClientWindow", "\350\276\223\345\205\245\345\247\223\345\220\215\346\210\226\345\255\246\345\217\267\346\220\234\347\264\242...", nullptr));
        searchButton->setText(QCoreApplication::translate("ClientWindow", "\346\237\245\350\257\242", nullptr));
        addButton->setText(QCoreApplication::translate("ClientWindow", "\346\267\273\345\212\240", nullptr));
        deleteButton->setText(QCoreApplication::translate("ClientWindow", "\345\210\240\351\231\244", nullptr));
        updateButton->setText(QCoreApplication::translate("ClientWindow", "\346\233\264\346\226\260", nullptr));
        addToExportBtn->setText(QCoreApplication::translate("ClientWindow", "\346\267\273\345\212\240\350\207\263\345\257\274\345\207\272\345\210\227\350\241\250", nullptr));
        label_format->setText(QCoreApplication::translate("ClientWindow", "\346\240\274\345\274\217:", nullptr));
        formatComboBox->setItemText(0, QCoreApplication::translate("ClientWindow", "XLSX", nullptr));
        formatComboBox->setItemText(1, QCoreApplication::translate("ClientWindow", "CSV", nullptr));

        mainTabWidget->setTabText(mainTabWidget->indexOf(tab), QCoreApplication::translate("ClientWindow", "\345\255\246\347\224\237\347\256\241\347\220\206", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(tab_2), QCoreApplication::translate("ClientWindow", "\346\225\260\346\215\256\347\273\237\350\256\241", nullptr));
        groupBox_exportPath->setTitle(QCoreApplication::translate("ClientWindow", "\345\257\274\345\207\272\350\256\276\347\275\256", nullptr));
        label_exportPath->setText(QCoreApplication::translate("ClientWindow", "\350\276\223\345\207\272\350\267\257\345\276\204:", nullptr));
        exportPathEdit->setPlaceholderText(QCoreApplication::translate("ClientWindow", "\350\257\267\351\200\211\346\213\251\345\257\274\345\207\272\347\233\256\345\275\225", nullptr));
        browseExportPathBtn->setText(QCoreApplication::translate("ClientWindow", "\346\265\217\350\247\210...", nullptr));
        exportBtn->setText(QCoreApplication::translate("ClientWindow", "\345\257\274\345\207\272", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(tab_export), QCoreApplication::translate("ClientWindow", "\346\225\260\346\215\256\345\257\274\345\207\272", nullptr));
        statusLabel->setText(QCoreApplication::translate("ClientWindow", "\345\207\206\345\244\207\345\260\261\347\273\252", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientWindow: public Ui_ClientWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTWINDOW_H
