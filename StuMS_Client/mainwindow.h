#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>

class LoginWidget;
class StudentWidget;
class ChartWidget;
class ExportWidget;
class InfoWidget;
class StudentHomeWidget; // [新增] 前置声明

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // [修改] 增加 username 参数，用于在学生主页显示名字
    void onLoginSuccess(QString role, QString username);
    void onLogout();
    void onNavChanged(int index);

private:
    // --- 核心架构 ---
    QStackedWidget *m_centralStack; // Page0=登录, Page1=管理员后台, Page2=学生主页

    // --- 登录界面 ---
    LoginWidget *m_loginWidget;

    // --- 管理员主功能区 ---
    QWidget *m_mainDashboard;
    QWidget *m_leftPanel;
    QListWidget *m_navBar;
    QPushButton *m_btnLogout;
    QWidget *m_rightPanel;
    QLabel *m_headerTitle;
    QStackedWidget *m_contentStack;

    // --- 管理员子功能页 ---
    StudentWidget *m_studentPage;
    ChartWidget *m_chartPage;
    ExportWidget *m_exportPage;
    InfoWidget *m_infoPage;

    // --- [新增] 学生独立主页 ---
    StudentHomeWidget *m_studentHomeWidget;

    void initUI();
    void initStyle();
};

#endif // MAINWINDOW_H
