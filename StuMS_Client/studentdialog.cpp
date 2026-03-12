#include "studentdialog.h"
#include "ui_studentdialog.h"

#include <QMessageBox>

StudentDialog::StudentDialog(QWidget *parent, bool isEditMode)
    : QDialog(parent)
    , ui(new Ui::StudentDialog)
    , m_isEditMode(isEditMode)
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle(m_isEditMode ? "编辑学生信息" : "添加新学生");
    
    // 初始化性别下拉框
    ui->genderComboBox->addItems({"男", "女"});
    
    // 设置年龄范围
    ui->ageSpinBox->setRange(15, 40);
}

StudentDialog::~StudentDialog()
{
    delete ui;
}

QString StudentDialog::studentId() const { return ui->idLineEdit->text(); }
QString StudentDialog::name() const { return ui->nameLineEdit->text(); }
QString StudentDialog::gender() const { return ui->genderComboBox->currentText() == "男" ? "male" : "female"; }
int StudentDialog::age() const { return ui->ageSpinBox->value(); }
QString StudentDialog::department() const { return ui->deptLineEdit->text(); }
QString StudentDialog::major() const { return ui->majorLineEdit->text(); }
QString StudentDialog::className() const { return ui->classLineEdit->text(); }
QString StudentDialog::phone() const { return ui->phoneLineEdit->text(); }
QString StudentDialog::email() const { return ui->emailLineEdit->text(); }

void StudentDialog::setStudentData(const QString &id, const QString &name, 
                                 const QString &gender, int age, const QString &dept, 
                                 const QString &major, const QString &cls, 
                                 const QString &phone, const QString &email)
{
    ui->idLineEdit->setText(id);
    ui->nameLineEdit->setText(name);
    // ui->genderComboBox->setCurrentText(gender);
    ui->genderComboBox->setCurrentText(gender == "male" ? "男" : "女");
    ui->ageSpinBox->setValue(age);
    ui->deptLineEdit->setText(dept);
    ui->majorLineEdit->setText(major);
    ui->classLineEdit->setText(cls);
    ui->phoneLineEdit->setText(phone);
    ui->emailLineEdit->setText(email);
    
    // 编辑模式下学号不可修改
    ui->idLineEdit->setEnabled(!m_isEditMode);
}

bool StudentDialog::validateInput()
{
    if (studentId().isEmpty()) {
        QMessageBox::warning(this, "错误", "学号不能为空");
        ui->idLineEdit->setFocus();
        return false;
    }

    if (name().isEmpty()) {
        QMessageBox::warning(this, "错误", "姓名不能为空");
        ui->nameLineEdit->setFocus();
        return false;
    }



    return true;
}

void StudentDialog::accept()
{
    if (validateInput()) {
        QDialog::accept();
    }
}
