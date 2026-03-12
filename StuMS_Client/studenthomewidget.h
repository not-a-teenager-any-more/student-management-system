#ifndef STUDENTHOMEWIDGET_H
#define STUDENTHOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>

class StudentHomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StudentHomeWidget(QWidget *parent = nullptr);

    // 设置显示的欢迎信息
    void setStudentInfo(const QString &username);

private:
    void setupUi();

    QLabel *m_lblWelcome;
    QLabel *m_lblAccount;
};

#endif // STUDENTHOMEWIDGET_H
