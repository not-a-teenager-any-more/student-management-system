#include "mainwindow.h"
#include "loginwidget.h"
#include "studentwidget.h"
#include "chartwidget.h"
#include "exportwidget.h"
#include "infowidget.h"
#include "studenthomewidget.h" // [新增] 引入头文件
#include "networkmanager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    initStyle();
}

MainWindow::~MainWindow() {}

void MainWindow::initUI()
{
    this->resize(1200, 720);
    this->setWindowTitle("学生信息管理系统 v1.0");

    m_centralStack = new QStackedWidget(this);
    this->setCentralWidget(m_centralStack);

    // --- 1. 登录界面 (Index 0) ---
    m_loginWidget = new LoginWidget(this);
    connect(m_loginWidget, &LoginWidget::loginSuccess, this, &MainWindow::onLoginSuccess);
    m_centralStack->addWidget(m_loginWidget);

    // --- 2. 管理员主后台 (Index 1) ---
    m_mainDashboard = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(m_mainDashboard);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧面板
    m_leftPanel = new QWidget;
    m_leftPanel->setObjectName("LeftPanel");
    m_leftPanel->setFixedWidth(220);

    QVBoxLayout *leftLayout = new QVBoxLayout(m_leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    QLabel *appLogo = new QLabel("🎓 EduManager");
    appLogo->setObjectName("AppLogo");
    appLogo->setAlignment(Qt::AlignCenter);
    appLogo->setFixedHeight(60);

    m_navBar = new QListWidget;
    m_navBar->setFocusPolicy(Qt::NoFocus);
    m_navBar->addItem("👤  学生管理"); // Index 0
    m_navBar->addItem("📊  数据分析"); // Index 1
    m_navBar->addItem("📂  数据导出"); // Index 2
    m_navBar->addItem("ℹ️  系统通知"); // Index 3

    m_btnLogout = new QPushButton("🚪 退出登录");
    m_btnLogout->setObjectName("BtnLogout");
    m_btnLogout->setCursor(Qt::PointingHandCursor);
    connect(m_btnLogout, &QPushButton::clicked, this, &MainWindow::onLogout);

    leftLayout->addWidget(appLogo);
    leftLayout->addWidget(m_navBar);
    leftLayout->addStretch();
    leftLayout->addWidget(m_btnLogout);

    // 右侧面板
    m_rightPanel = new QWidget;
    m_rightPanel->setObjectName("RightPanel");
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    QWidget *headerContainer = new QWidget;
    headerContainer->setObjectName("HeaderContainer");
    headerContainer->setFixedHeight(60);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerContainer);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    m_headerTitle = new QLabel("首页");
    m_headerTitle->setObjectName("HeaderTitle");
    headerLayout->addWidget(m_headerTitle);
    headerLayout->addStretch();

    m_contentStack = new QStackedWidget;
    m_contentStack->setContentsMargins(10, 10, 10, 10);

    m_studentPage = new StudentWidget;
    m_chartPage = new ChartWidget;
    m_exportPage = new ExportWidget;
    m_infoPage = new InfoWidget;

    if (m_exportPage->getModel()) {
        m_studentPage->setExportModel(m_exportPage->getModel());
    }

    m_contentStack->addWidget(m_studentPage);
    m_contentStack->addWidget(m_chartPage);
    m_contentStack->addWidget(m_exportPage);
    m_contentStack->addWidget(m_infoPage);

    rightLayout->addWidget(headerContainer);
    rightLayout->addWidget(m_contentStack);

    mainLayout->addWidget(m_leftPanel);
    mainLayout->addWidget(m_rightPanel);

    m_centralStack->addWidget(m_mainDashboard);

    connect(m_navBar, &QListWidget::currentRowChanged, this, &MainWindow::onNavChanged);

    // --- 3. [新增] 学生独立主页 (Index 2) ---
    m_studentHomeWidget = new StudentHomeWidget(this);
    m_centralStack->addWidget(m_studentHomeWidget);
}

void MainWindow::initStyle()
{
    QString darkBg = "#2c3e50";
    QString darkItem = "#34495e";
    QString accentColor = "#1abc9c";
    QString textColor = "#ecf0f1";
    QString rightBg = "#f5f6fa";

    this->setStyleSheet(QString(
                            "QWidget { font-family: 'Microsoft YaHei', sans-serif; font-size: 14px; }"
                            "QWidget#LeftPanel { background-color: %1; }"
                            "QLabel#AppLogo { color: #ffffff; font-size: 20px; font-weight: bold; border-bottom: 1px solid %2; }"
                            "QListWidget { background-color: transparent; border: none; outline: none; margin-top: 10px; }"
                            "QListWidget::item { height: 50px; color: %4; padding-left: 15px; border-left: 4px solid transparent; }"
                            "QListWidget::item:hover { background-color: %2; }"
                            "QListWidget::item:selected { background-color: %2; color: #ffffff; border-left: 4px solid %3; }"
                            "QPushButton#BtnLogout { text-align: left; padding-left: 20px; height: 50px; border: none; color: #e74c3c; background-color: transparent; font-weight: bold; }"
                            "QPushButton#BtnLogout:hover { background-color: %2; }"
                            "QWidget#RightPanel { background-color: %5; }"
                            "QWidget#HeaderContainer { background-color: #ffffff; border-bottom: 1px solid #dcdde1; }"
                            "QLabel#HeaderTitle { font-size: 18px; color: #2f3640; font-weight: bold; }"
                            "QStackedWidget { background-color: transparent; }"
                            ).arg(darkBg, darkItem, accentColor, textColor, rightBg));
}

// [核心逻辑] 登录成功跳转
void MainWindow::onLoginSuccess(QString role, QString username)
{
    if (role == "admin") {
        // 如果是管理员，跳转到后台 (Index 1)
        m_centralStack->setCurrentWidget(m_mainDashboard);
        m_navBar->setCurrentRow(0);
        m_studentPage->updatePermissions(role); // 确保管理页权限正常
    }
    else {
        // 如果是学生，跳转到静态主页 (Index 2)
        m_studentHomeWidget->setStudentInfo(username); // 设置名字
        m_centralStack->setCurrentWidget(m_studentHomeWidget);
    }
}

void MainWindow::onNavChanged(int index)
{
    m_contentStack->setCurrentIndex(index);
    QString title = m_navBar->item(index)->text();
    m_headerTitle->setText(title);

    if (index == 0) {
        m_studentPage->refreshData();
    }
    else if (index == 1) {
        NetworkManager::instance().fetchStudents();
    }
}

void MainWindow::onLogout()
{
    m_centralStack->setCurrentIndex(0); // 返回登录页
    m_navBar->clearSelection();
}
