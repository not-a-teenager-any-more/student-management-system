#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>

namespace Ui {
class StudentDialog;
}

class StudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StudentDialog(QWidget *parent = nullptr, bool isEditMode = false);
    ~StudentDialog();

    QString studentId() const;
    QString name() const;
    QString gender() const;
    int age() const;
    QString department() const;
    QString major() const;
    QString className() const;
    QString phone() const;
    QString email() const;

    void setStudentData(const QString &id, const QString &name, const QString &gender, 
                      int age, const QString &dept, const QString &major, 
                      const QString &cls, const QString &phone, const QString &email);

    bool validateInput();


public slots:
    void accept();
private:
    Ui::StudentDialog *ui;
    bool m_isEditMode;
};

#endif // STUDENTDIALOG_H
