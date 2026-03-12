/********************************************************************************
** Form generated from reading UI file 'studentdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STUDENTDIALOG_H
#define UI_STUDENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_StudentDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *idLineEdit;
    QLabel *label_2;
    QLineEdit *nameLineEdit;
    QLabel *label_3;
    QComboBox *genderComboBox;
    QLabel *label_4;
    QSpinBox *ageSpinBox;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_5;
    QLineEdit *deptLineEdit;
    QLabel *label_6;
    QLineEdit *majorLineEdit;
    QLabel *label_7;
    QLineEdit *classLineEdit;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_3;
    QLabel *label_8;
    QLineEdit *phoneLineEdit;
    QLabel *label_9;
    QLineEdit *emailLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *StudentDialog)
    {
        if (StudentDialog->objectName().isEmpty())
            StudentDialog->setObjectName("StudentDialog");
        StudentDialog->resize(400, 450);
        verticalLayout = new QVBoxLayout(StudentDialog);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(StudentDialog);
        groupBox->setObjectName("groupBox");
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        idLineEdit = new QLineEdit(groupBox);
        idLineEdit->setObjectName("idLineEdit");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, idLineEdit);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        nameLineEdit = new QLineEdit(groupBox);
        nameLineEdit->setObjectName("nameLineEdit");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, nameLineEdit);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        genderComboBox = new QComboBox(groupBox);
        genderComboBox->setObjectName("genderComboBox");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, genderComboBox);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_4);

        ageSpinBox = new QSpinBox(groupBox);
        ageSpinBox->setObjectName("ageSpinBox");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, ageSpinBox);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(StudentDialog);
        groupBox_2->setObjectName("groupBox_2");
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName("formLayout_2");
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, label_5);

        deptLineEdit = new QLineEdit(groupBox_2);
        deptLineEdit->setObjectName("deptLineEdit");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, deptLineEdit);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_6);

        majorLineEdit = new QLineEdit(groupBox_2);
        majorLineEdit->setObjectName("majorLineEdit");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, majorLineEdit);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName("label_7");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, label_7);

        classLineEdit = new QLineEdit(groupBox_2);
        classLineEdit->setObjectName("classLineEdit");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, classLineEdit);


        verticalLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(StudentDialog);
        groupBox_3->setObjectName("groupBox_3");
        formLayout_3 = new QFormLayout(groupBox_3);
        formLayout_3->setObjectName("formLayout_3");
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName("label_8");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::LabelRole, label_8);

        phoneLineEdit = new QLineEdit(groupBox_3);
        phoneLineEdit->setObjectName("phoneLineEdit");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::FieldRole, phoneLineEdit);

        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName("label_9");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::LabelRole, label_9);

        emailLineEdit = new QLineEdit(groupBox_3);
        emailLineEdit->setObjectName("emailLineEdit");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::FieldRole, emailLineEdit);


        verticalLayout->addWidget(groupBox_3);

        buttonBox = new QDialogButtonBox(StudentDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(StudentDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, StudentDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, StudentDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(StudentDialog);
    } // setupUi

    void retranslateUi(QDialog *StudentDialog)
    {
        StudentDialog->setWindowTitle(QCoreApplication::translate("StudentDialog", "\345\255\246\347\224\237\344\277\241\346\201\257", nullptr));
        groupBox->setTitle(QCoreApplication::translate("StudentDialog", "\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        label->setText(QCoreApplication::translate("StudentDialog", "\345\255\246\345\217\267\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("StudentDialog", "\345\247\223\345\220\215\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("StudentDialog", "\346\200\247\345\210\253\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("StudentDialog", "\345\271\264\351\276\204\357\274\232", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("StudentDialog", "\351\231\242\347\263\273\344\277\241\346\201\257", nullptr));
        label_5->setText(QCoreApplication::translate("StudentDialog", "\351\231\242\347\263\273\357\274\232", nullptr));
        label_6->setText(QCoreApplication::translate("StudentDialog", "\344\270\223\344\270\232\357\274\232", nullptr));
        label_7->setText(QCoreApplication::translate("StudentDialog", "\347\217\255\347\272\247\357\274\232", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("StudentDialog", "\350\201\224\347\263\273\346\226\271\345\274\217", nullptr));
        label_8->setText(QCoreApplication::translate("StudentDialog", "\347\224\265\350\257\235\357\274\232", nullptr));
        label_9->setText(QCoreApplication::translate("StudentDialog", "\351\202\256\347\256\261\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StudentDialog: public Ui_StudentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STUDENTDIALOG_H
