#include "loginwidget.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QShowEvent> // [新增]
#include <QDebug>

// ==========================================
//  ImageCaptcha 实现
// ==========================================
ImageCaptcha::ImageCaptcha(QWidget *parent) : QWidget(parent)
{
    setFixedSize(100, 42);
    setCursor(Qt::PointingHandCursor);
    setToolTip("点击刷新验证码");
    m_colors << Qt::black << Qt::red << Qt::blue << Qt::darkGreen << Qt::darkMagenta;
    regenerate();
}

void ImageCaptcha::regenerate()
{
    const QString chars = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
    m_code.clear();
    for(int i=0; i<4; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        m_code.append(chars.at(index));
    }
    update();
}

void ImageCaptcha::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor("#f8f9fa"));
    painter.setPen(QColor("#e0e0e0"));
    painter.drawRect(rect().adjusted(0,0,-1,-1));

    for(int i=0; i<40; ++i) {
        painter.setPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())]);
        painter.drawPoint(QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()));
    }
    for(int i=0; i<6; ++i) {
        painter.setPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())]);
        painter.drawLine(QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()),
                         QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()));
    }

    QFont font("Arial", 18, QFont::Bold);
    painter.setFont(font);
    int charWidth = width() / 4;
    for(int i=0; i<4; ++i) {
        painter.setPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())]);
        painter.save();
        int x = i * charWidth + charWidth/2 - 8;
        int y = height() / 2 + 8;
        painter.translate(x, y);
        painter.rotate(QRandomGenerator::global()->bounded(-20, 20));
        painter.drawText(0, 0, QString(m_code.at(i)));
        painter.restore();
    }
}

void ImageCaptcha::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        regenerate();
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

// ==========================================
//  LoginWidget 实现
// ==========================================

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent)
{
    resize(1000, 650);
    setWindowTitle("学生信息管理系统");
    setFocusPolicy(Qt::StrongFocus);

    m_regWidget = nullptr;
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &LoginWidget::onLoginTimeout);

    setupUi();
    setupStyle();

    setTabOrder(m_userEdit, m_passEdit);
    setTabOrder(m_passEdit, m_captchaEdit);
    setTabOrder(m_captchaEdit, m_loginBtn);

    NetworkManager &net = NetworkManager::instance();
    connect(&net, &NetworkManager::loginResult, this, [this](bool success, QString role, QString msg){
        m_timeoutTimer->stop();
        if(success) {
            m_loginBtn->setText("登录成功");
            m_msgLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
            m_msgLabel->setText("✓ 验证成功，正在跳转...");

            // 登录成功后，稍作延迟再关闭，体验更好
            QTimer::singleShot(800, [this, role](){
                emit loginSuccess(role, m_userEdit->text());
                this->close();
            });
        } else {
            handleLoginFailed(msg);
        }
    });

    connect(&net, &NetworkManager::connectionError, this, [this](QString msg){
        m_timeoutTimer->stop();
        handleLoginFailed(msg);
    });
}

LoginWidget::~LoginWidget() {
    if(m_regWidget) delete m_regWidget;
}

// [核心修复] 当窗口再次显示时（例如从主界面退出重新登录），重置所有状态
void LoginWidget::showEvent(QShowEvent *event)
{
    // 1. 重置按钮状态
    m_loginBtn->setEnabled(true);
    m_loginBtn->setText("登录");

    // 2. 清空密码和验证码，但保留用户名（方便用户）
    m_passEdit->clear();
    m_captchaEdit->clear();
    m_msgLabel->clear(); // 清除之前的错误或成功信息

    // 3. 刷新验证码
    m_captchaImg->regenerate();

    // 4. 让用户名框或密码框获得焦点
    if(m_userEdit->text().isEmpty()) {
        m_userEdit->setFocus();
    } else {
        m_passEdit->setFocus();
    }

    QWidget::showEvent(event);
}

void LoginWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape) {
        close();
    }
    else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if(!m_loginBtn->hasFocus()) {
            onLoginClicked();
        }
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

// [核心功能] 处理失败：不弹窗，直接显示红字 + 窗口抖动
void LoginWidget::handleLoginFailed(const QString &msg)
{
    // 恢复按钮可用状态
    m_loginBtn->setEnabled(true);
    m_loginBtn->setText("登录");

    // 界面显示红色错误信息
    m_msgLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    m_msgLabel->setText(msg);

    // 刷新验证码，防止暴力破解
    m_captchaImg->regenerate();
    m_captchaEdit->clear();

    // 窗口左右抖动动画
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(500);
    animation->setLoopCount(1);
    // 设定抖动轨迹
    animation->setKeyValueAt(0, pos());
    animation->setKeyValueAt(0.1, pos() + QPoint(10, 0));
    animation->setKeyValueAt(0.2, pos() - QPoint(10, 0));
    animation->setKeyValueAt(0.3, pos() + QPoint(10, 0));
    animation->setKeyValueAt(0.4, pos());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginWidget::onLoginTimeout()
{
    if(isVisible()) {
        handleLoginFailed("连接服务器超时，请检查网络设置");
    }
}

void LoginWidget::onLoginClicked() {
    if(m_userEdit->text().trimmed().isEmpty()) {
        m_msgLabel->setStyleSheet("color: #e74c3c;");
        m_msgLabel->setText("请输入账号"); m_userEdit->setFocus(); return;
    }
    if(m_passEdit->text().isEmpty()) {
        m_msgLabel->setStyleSheet("color: #e74c3c;");
        m_msgLabel->setText("请输入密码"); m_passEdit->setFocus(); return;
    }
    if(m_captchaEdit->text().isEmpty()) {
        m_msgLabel->setStyleSheet("color: #e74c3c;");
        m_msgLabel->setText("请输入验证码"); m_captchaEdit->setFocus(); return;
    }

    if(m_captchaEdit->text().compare(m_captchaImg->getCode(), Qt::CaseInsensitive) != 0) {
        // 验证码错误直接调用失败处理（内含抖动和刷新）
        handleLoginFailed("验证码错误");
        return;
    }

    m_loginBtn->setText("正在连接...");
    m_loginBtn->setEnabled(false); // 此时禁用，防止重复点击
    m_msgLabel->setText("");
    m_timeoutTimer->start(3000);

    NetworkManager::instance().connectToHost(m_ipEdit->text(), m_portEdit->text().toUShort());

    QJsonObject req;
    req["type"] = "login";
    QJsonObject data;
    data["username"] = m_userEdit->text().trimmed();
    data["password"] = m_passEdit->text();
    data["role"] = (m_roleGroup->checkedId() == 0) ? "student" : "admin";
    req["data"] = data;

    QTimer::singleShot(100, [req](){
        NetworkManager::instance().sendRequest(req);
    });
}

void LoginWidget::setupUi() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setupLeftPanel();
    setupRightPanel();
    mainLayout->addWidget(m_leftPanel, 4);
    mainLayout->addWidget(m_rightPanel, 6);
}

void LoginWidget::setupLeftPanel() {
    m_leftPanel = new QWidget(this);
    m_leftPanel->setObjectName("LeftPanel");
    QVBoxLayout *layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(40, 0, 40, 0);
    layout->addStretch();
    QLabel *icon = new QLabel("🎓");
    icon->setStyleSheet("font-size: 80px; background: transparent;");
    icon->setAlignment(Qt::AlignCenter);
    QLabel *logo = new QLabel("EDU SYSTEM");
    logo->setStyleSheet("font-size: 32px; font-weight: bold; color: white; margin-top: 10px;");
    logo->setAlignment(Qt::AlignCenter);
    QLabel *desc = new QLabel("高效 · 智能 · 便捷\nOne Stop Campus Solution");
    desc->setStyleSheet("font-size: 16px; color: #bdc3c7; line-height: 24px; margin-top: 20px;");
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(icon);
    layout->addWidget(logo);
    layout->addWidget(desc);
    layout->addStretch();
}

void LoginWidget::setupRightPanel() {
    m_rightPanel = new QWidget(this);
    m_rightPanel->setObjectName("RightPanel");
    QVBoxLayout *layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(100, 50, 100, 50);
    layout->setSpacing(20);

    QLabel *title = new QLabel("欢迎回来");
    title->setObjectName("LoginTitle");

    // --- 身份选择区域 ---
    m_roleContainer = new QWidget();
    QHBoxLayout *roleLayout = new QHBoxLayout(m_roleContainer);
    roleLayout->setContentsMargins(0,0,0,0);
    roleLayout->setSpacing(20);
    m_roleGroup = new QButtonGroup(this);
    m_btnStudent = new QPushButton("学生登录");
    m_btnStudent->setCheckable(true); m_btnStudent->setChecked(true);
    m_btnStudent->setObjectName("RoleBtn"); m_btnStudent->setCursor(Qt::PointingHandCursor);
    m_btnAdmin = new QPushButton("管理员");
    m_btnAdmin->setCheckable(true);
    m_btnAdmin->setObjectName("RoleBtn"); m_btnAdmin->setCursor(Qt::PointingHandCursor);
    m_roleGroup->addButton(m_btnStudent, 0);
    m_roleGroup->addButton(m_btnAdmin, 1);
    connect(m_roleGroup, &QButtonGroup::idClicked, this, &LoginWidget::onRoleChanged);
    roleLayout->addWidget(m_btnStudent);
    roleLayout->addWidget(m_btnAdmin);
    roleLayout->addStretch();

    // --- 账号输入 ---
    m_userEdit = new QLineEdit();
    m_userEdit->setPlaceholderText("请输入学号 / 用户名");
    m_userEdit->setFixedHeight(50);
    m_userEdit->setClearButtonEnabled(true);
    m_userEdit->setObjectName("NormalEdit");

    // --- 密码输入组合 ---
    m_passContainer = new QWidget();
    m_passContainer->setObjectName("PassContainer");
    m_passContainer->setFixedHeight(50);
    QHBoxLayout *passLayout = new QHBoxLayout(m_passContainer);
    passLayout->setContentsMargins(10, 0, 5, 0);
    passLayout->setSpacing(5);

    m_passEdit = new QLineEdit();
    m_passEdit->setPlaceholderText("请输入密码");
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setFrame(false);
    m_passEdit->setStyleSheet("background: transparent; border: none; font-size: 15px;");

    m_eyeBtn = new QToolButton();
    m_eyeBtn->setText("👁");
    m_eyeBtn->setCursor(Qt::PointingHandCursor);
    m_eyeBtn->setFixedSize(40, 40);
    m_eyeBtn->setStyleSheet("QToolButton { border: none; background: transparent; font-size: 20px; color: #95a5a6; } QToolButton:hover { color: #3498db; }");
    connect(m_eyeBtn, &QToolButton::clicked, this, &LoginWidget::togglePasswordVisibility);

    passLayout->addWidget(m_passEdit);
    passLayout->addWidget(m_eyeBtn);

    // --- 验证码区域 ---
    QWidget *captchaContainer = new QWidget();
    QHBoxLayout *captchaLayout = new QHBoxLayout(captchaContainer);
    captchaLayout->setContentsMargins(0,0,0,0);
    captchaLayout->setSpacing(10);
    m_captchaEdit = new QLineEdit();
    m_captchaEdit->setPlaceholderText("验证码");
    m_captchaEdit->setFixedHeight(42);
    m_captchaEdit->setMaxLength(4);
    m_captchaEdit->setObjectName("NormalEdit");

    m_captchaImg = new ImageCaptcha(this);

    captchaLayout->addWidget(m_captchaEdit, 1);
    captchaLayout->addWidget(m_captchaImg, 0);

    // --- 登录按钮 ---
    m_loginBtn = new QPushButton("登录");
    m_loginBtn->setFixedHeight(55);
    m_loginBtn->setCursor(Qt::PointingHandCursor);

    m_msgLabel = new QLabel("");
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setFixedHeight(20);

    // --- 底部设置与注册 ---
    m_settingsBtn = new QPushButton("服务器设置"); m_settingsBtn->setObjectName("LinkBtn");
    m_goToRegBtn = new QPushButton("注册新账号"); m_goToRegBtn->setObjectName("LinkBtn");
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_settingsBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_goToRegBtn);

    // --- 隐藏的服务器设置区域 ---
    m_settingsContainer = new QWidget();
    QHBoxLayout *setLay = new QHBoxLayout(m_settingsContainer);
    setLay->setContentsMargins(0,0,0,0);
    m_ipEdit = new QLineEdit("127.0.0.1"); m_ipEdit->setPlaceholderText("IP Address");
    m_portEdit = new QLineEdit("12345"); m_portEdit->setPlaceholderText("Port");
    m_ipEdit->setObjectName("NormalEdit"); m_portEdit->setObjectName("NormalEdit");
    setLay->addWidget(m_ipEdit); setLay->addWidget(m_portEdit);
    m_settingsContainer->setVisible(false);

    // --- 添加到主布局 ---
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(m_roleContainer);
    layout->addSpacing(10);
    layout->addWidget(m_userEdit);
    layout->addWidget(m_passContainer);
    layout->addSpacing(5);
    layout->addWidget(captchaContainer);
    layout->addSpacing(15);
    layout->addWidget(m_loginBtn);
    layout->addWidget(m_msgLabel);
    layout->addSpacing(10);
    layout->addLayout(bottomLayout);
    layout->addWidget(m_settingsContainer);
    layout->addStretch();

    // --- 信号连接 ---
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(m_settingsBtn, &QPushButton::clicked, this, &LoginWidget::toggleServerSettings);

    // 回车自动跳跃逻辑 (Focus Chain)
    connect(m_userEdit, &QLineEdit::returnPressed, m_passEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_passEdit, &QLineEdit::returnPressed, m_captchaEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_captchaEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);

    // [关键修改] 注册按钮点击逻辑：传递服务器信息
    connect(m_goToRegBtn, &QPushButton::clicked, this, [this](){
        if(!m_regWidget) {
            m_regWidget = new RegisterWidget();
            // 注册界面返回时重新显示登录界面
            connect(m_regWidget, &RegisterWidget::goBackToLogin, this, [this](){ this->show(); });
        }

        // 1. 尝试连接服务器（为注册做准备）
        NetworkManager::instance().connectToHost(m_ipEdit->text(), m_portEdit->text().toUShort());

        // 2. 将当前配置的 IP 和 端口 传递给注册界面，以便它断线重连
        m_regWidget->setServerInfo(m_ipEdit->text(), m_portEdit->text().toUShort());

        m_regWidget->show();
        this->hide();
    });
}
void LoginWidget::togglePasswordVisibility() {
    if(m_passEdit->echoMode() == QLineEdit::Password) {
        m_passEdit->setEchoMode(QLineEdit::Normal);
        m_eyeBtn->setText("🚫");
        m_eyeBtn->setStyleSheet("QToolButton { border: none; background: transparent; font-size: 20px; color: #e74c3c; }");
    } else {
        m_passEdit->setEchoMode(QLineEdit::Password);
        m_eyeBtn->setText("👁");
        m_eyeBtn->setStyleSheet("QToolButton { border: none; background: transparent; font-size: 20px; color: #95a5a6; } QToolButton:hover { color: #3498db; }");
    }
    m_passEdit->setFocus();
    m_passEdit->setCursorPosition(m_passEdit->text().length());
}

void LoginWidget::onRoleChanged(int id) {
    m_msgLabel->clear();
    if (id == 0) m_userEdit->setPlaceholderText("请输入学号");
    else         m_userEdit->setPlaceholderText("请输入管理员账号");
    m_userEdit->setFocus();
}

void LoginWidget::toggleServerSettings() {
    m_isSettingsVisible = !m_isSettingsVisible;
    m_settingsContainer->setVisible(m_isSettingsVisible);
}

void LoginWidget::setupStyle() {
    QString qss = R"(
        #LeftPanel { background-color: #2c3e50; }
        #RightPanel { background-color: #ffffff; }
        #LoginTitle { font-size: 32px; color: #2c3e50; font-family: "Microsoft YaHei"; font-weight: bold; }

        #RoleBtn { background: transparent; color: #95a5a6; font-size: 16px; border: none; padding-bottom: 5px; border-bottom: 3px solid transparent; font-weight: bold; }
        #RoleBtn:checked { color: #3498db; border-bottom: 3px solid #3498db; }
        #RoleBtn:hover { color: #3498db; }

        #NormalEdit { border: 2px solid #f0f2f5; border-radius: 8px; background: #fcfcfc; font-size: 15px; padding: 0 10px; selection-background-color: #3498db; }
        #NormalEdit:focus { border: 2px solid #3498db; background: #ffffff; }

        #PassContainer { border: 2px solid #f0f2f5; border-radius: 8px; background: #fcfcfc; }
        #PassContainer:hover { border: 2px solid #bdc3c7; }

        QPushButton { border-radius: 8px; font-family: "Microsoft YaHei"; }
        QPushButton:!flat { background-color: #3498db; color: white; font-size: 18px; font-weight: bold; border: none; }
        QPushButton:!flat:hover { background-color: #2980b9; }
        QPushButton:!flat:pressed { background-color: #1f618d; }
        QPushButton:disabled { background-color: #bdc3c7; color: #fdfdfd; }

        #LinkBtn { background: transparent; color: #7f8c8d; border: none; font-size: 14px; }
        #LinkBtn:hover { color: #3498db; text-decoration: underline; }
    )";
    this->setStyleSheet(qss);
}

void LoginWidget::mousePressEvent(QMouseEvent *event) {
    setFocus();
    QWidget::mousePressEvent(event);
}
