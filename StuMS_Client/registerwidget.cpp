#include "registerwidget.h"
#include "networkmanager.h" // 确保你的项目中包含这个单例类

// --- 必须的引用 ---
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument> // [重要] 处理JSON必须
#include <QPainter>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QDebug>
#include <QRegularExpression>

// ==========================================
//  验证码组件 (保持不变)
// ==========================================
CaptchaLabel::CaptchaLabel(QWidget *parent) : QLabel(parent)
{
    setFixedSize(100, 42);
    setCursor(Qt::PointingHandCursor);
    setToolTip("点击刷新");
    m_colors << QColor("#2c3e50") << QColor("#8e44ad") << QColor("#16a085") << QColor("#c0392b");
    regenerate();
}

void CaptchaLabel::regenerate() {
    const QString chars = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
    m_code.clear();
    for(int i=0; i<4; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        m_code.append(chars.at(index));
    }
    update();
}

void CaptchaLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) regenerate();
}

void CaptchaLabel::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#f9f9f9"));
    painter.setPen(QColor("#e0e0e0"));
    painter.drawRect(rect().adjusted(0,0,-1,-1));

    for(int i=0; i<30; ++i) {
        painter.setPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())]);
        painter.drawPoint(QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()));
    }
    // 简单画点干扰线
    for(int i=0; i<4; ++i) {
        painter.setPen(QPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())], 1));
        painter.drawLine(QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()),
                         QRandomGenerator::global()->bounded(width()), QRandomGenerator::global()->bounded(height()));
    }

    QFont font("Georgia", 18, QFont::Bold);
    painter.setFont(font);
    int charWidth = width() / 4;
    for(int i=0; i<4; ++i) {
        painter.setPen(m_colors[QRandomGenerator::global()->bounded(m_colors.size())]);
        painter.save();
        int x = i * charWidth + charWidth/2 - 6;
        int y = height() / 2 + 6;
        painter.translate(x, y);
        painter.rotate(QRandomGenerator::global()->bounded(-15, 15));
        painter.drawText(0, 0, QString(m_code.at(i)));
        painter.restore();
    }
}

// ==========================================
//  RegisterWidget 实现
// ==========================================

RegisterWidget::RegisterWidget(QWidget *parent) : QWidget(parent)
{
    resize(1000, 720); // 增加高度以适应标签
    setWindowTitle("注册新用户 - 智慧校园系统");
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_isUserValid = false;

    // [核心] 初始化防抖定时器
    // 作用：用户输入时不断重置，直到用户停止输入 500ms 后才触发 timeout 信号
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true); // 只触发一次
    m_debounceTimer->setInterval(500);    // 500ms 延迟
    connect(m_debounceTimer, &QTimer::timeout, this, &RegisterWidget::sendCheckUserRequest);

    setupUi();
    setupStyle();

    // 绑定注册结果 (保持原有逻辑)
    connect(&NetworkManager::instance(), &NetworkManager::registrationResult,
            this, [this](bool success, QString msg){
                m_regBtn->setText("立即注册");
                m_regBtn->setEnabled(true);

                if(success) {
                    m_msgLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
                    m_msgLabel->setText("✓ 注册成功! 即将跳转登录...");
                    m_rightPanel->setEnabled(false); // 禁用面板防止重复点击

                    QTimer::singleShot(1500, this, [this](){
                        emit goBackToLogin();
                        this->hide();
                        m_rightPanel->setEnabled(true);
                    });
                } else {
                    m_msgLabel->setStyleSheet("color: #c0392b; font-weight: bold;");
                    m_msgLabel->setText(msg);
                    m_captchaLabel->regenerate();
                    m_captchaEdit->clear();

                    // 震动动画提示错误
                    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
                    animation->setDuration(500);
                    animation->setLoopCount(1);
                    animation->setKeyValueAt(0, pos());
                    animation->setKeyValueAt(0.1, pos() + QPoint(10, 0));
                    animation->setKeyValueAt(0.2, pos() - QPoint(10, 0));
                    animation->setKeyValueAt(0.3, pos() + QPoint(10, 0));
                    animation->setKeyValueAt(0.4, pos());
                    animation->start(QAbstractAnimation::DeleteWhenStopped);
                }
            });

    // [关键] 连接 NetworkManager 的检查结果信号
    // 注意：你必须在 NetworkManager 中定义 checkUserResult 信号
    connect(&NetworkManager::instance(), &NetworkManager::checkUserResult,
            this, &RegisterWidget::onUserCheckResult);
}

void RegisterWidget::setServerInfo(const QString &ip, quint16 port)
{
    m_serverIp = ip;
    m_serverPort = port;
}

void RegisterWidget::closeEvent(QCloseEvent *event)
{
    emit goBackToLogin();
    event->accept();
}

void RegisterWidget::showEvent(QShowEvent *event) {
    m_userEdit->clear();
    m_passEdit->clear();
    m_confirmPassEdit->clear();
    m_captchaEdit->clear();
    m_msgLabel->clear();

    // 重置状态
    m_userStatusLabel->clear();
    m_isUserValid = false;
    m_debounceTimer->stop();

    m_captchaLabel->regenerate();
    m_roleCombo->setCurrentIndex(0);
    onRoleChanged(0);
    m_regBtn->setEnabled(true);
    m_regBtn->setText("立即注册");

    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEyeBtn->setText("👁");
    m_confirmPassEdit->setEchoMode(QLineEdit::Password);
    m_confirmEyeBtn->setText("👁");

    m_userEdit->setFocus();
    QWidget::showEvent(event);
}

void RegisterWidget::setupUi() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setupLeftPanel();
    setupRightPanel();
    mainLayout->addWidget(m_leftPanel, 4);
    mainLayout->addWidget(m_rightPanel, 6);
}

void RegisterWidget::setupLeftPanel() {
    // 左侧面板代码与之前保持一致...
    m_leftPanel = new QWidget(this);
    m_leftPanel->setObjectName("RegLeftPanel");
    QVBoxLayout *layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(50, 0, 50, 0);
    layout->addStretch();
    QLabel *title = new QLabel("Create\nAccount");
    title->setStyleSheet("font-size: 48px; font-weight: bold; color: white; font-family: 'Arial'; line-height: 55px;");
    QLabel *subTitle = new QLabel("加入智慧校园");
    subTitle->setStyleSheet("font-size: 24px; color: #ecf0f1; margin-top: 20px; font-weight: 300;");
    QWidget *line = new QWidget();
    line->setFixedHeight(2);
    line->setFixedWidth(60);
    line->setStyleSheet("background-color: rgba(255,255,255,0.5); margin-top: 20px;");
    QLabel *desc = new QLabel("Start your journey today.\nEfficient · Intelligent · Convenient");
    desc->setStyleSheet("font-size: 14px; color: #bdc3c7; margin-top: 30px; line-height: 24px;");
    layout->addWidget(title);
    layout->addWidget(subTitle);
    layout->addWidget(line);
    layout->addWidget(desc);
    layout->addStretch();
}

// [辅助函数] 添加 "标题 + 状态" 的布局
// 返回状态 Label 指针，以便后续更新
QLabel* RegisterWidget::addInputLabel(QVBoxLayout *layout, const QString &text) {
    QWidget *container = new QWidget();
    QHBoxLayout *hBox = new QHBoxLayout(container);
    hBox->setContentsMargins(0, 5, 0, 2); // 微调间距
    hBox->setSpacing(10);

    QLabel *titleLbl = new QLabel(text);
    titleLbl->setObjectName("FormLabel"); // 用于QSS样式

    QLabel *statusLbl = new QLabel(""); // 初始为空
    statusLbl->setObjectName("StatusLabel");
    statusLbl->setStyleSheet("font-size: 12px; font-weight: bold;");

    hBox->addWidget(titleLbl);
    hBox->addWidget(statusLbl);
    hBox->addStretch(); // 让标签靠左

    layout->addWidget(container);
    return statusLbl;
}

// 请替换 RegisterWidget.cpp 中的 setupRightPanel 函数
void RegisterWidget::setupRightPanel() {
    m_rightPanel = new QWidget(this);
    m_rightPanel->setObjectName("RegRightPanel");

    QVBoxLayout *layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(60, 40, 60, 40);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop); // 【关键】让所有控件向上对齐，避免分散

    // 1. 顶部 Header (注册 + 登录链接)
    QHBoxLayout *topLinkLayout = new QHBoxLayout();
    QLabel *header = new QLabel("注册");
    header->setObjectName("RegHeader");
    topLinkLayout->addWidget(header);
    topLinkLayout->addStretch();
    QLabel *subText = new QLabel("已有账号?");
    subText->setObjectName("RegSubText");
    QPushButton *loginLink = new QPushButton("直接登录");
    loginLink->setObjectName("LinkBtn");
    loginLink->setCursor(Qt::PointingHandCursor);
    connect(loginLink, &QPushButton::clicked, this, [this](){
        emit goBackToLogin();
        this->hide();
    });
    topLinkLayout->addWidget(subText);
    topLinkLayout->addWidget(loginLink);

    // 将顶部 Header 先加入布局
    layout->addLayout(topLinkLayout);
    layout->addSpacing(20); // 增加一点间距

    // 2. 用户名区域 (严格按顺序添加)
    // 先加标签
    m_userStatusLabel = addInputLabel(layout, "用户名");
    // 再加输入框
    m_userEdit = new QLineEdit();
    m_userEdit->setPlaceholderText("请输入用户名 (3-12位)");
    m_userEdit->setFixedHeight(45);
    m_userEdit->setObjectName("NormalEdit");
    connect(m_userEdit, &QLineEdit::textChanged, this, &RegisterWidget::onUserTextChanged);
    layout->addWidget(m_userEdit); // 【关键】紧跟标签之后添加
    layout->addSpacing(5);

    // 3. 密码区域
    addInputLabel(layout, "设置密码");
    m_passContainer = new QWidget();
    m_passContainer->setObjectName("PassContainer");
    m_passContainer->setFixedHeight(45);
    QHBoxLayout *passLayout = new QHBoxLayout(m_passContainer);
    passLayout->setContentsMargins(10, 0, 5, 0);
    m_passEdit = new QLineEdit();
    m_passEdit->setPlaceholderText("至少6位字符");
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setFrame(false);
    m_passEdit->setStyleSheet("background: transparent; border: none; font-size: 15px;");
    m_passEyeBtn = new QToolButton();
    m_passEyeBtn->setText("👁");
    m_passEyeBtn->setCursor(Qt::PointingHandCursor);
    m_passEyeBtn->setFixedSize(40, 40);
    m_passEyeBtn->setStyleSheet("border: none; background: transparent; color: #95a5a6; font-size: 16px;");
    connect(m_passEyeBtn, &QToolButton::clicked, this, &RegisterWidget::togglePassVisibility);
    passLayout->addWidget(m_passEdit);
    passLayout->addWidget(m_passEyeBtn);
    layout->addWidget(m_passContainer); // 【关键】加入布局
    layout->addSpacing(5);

    // 4. 确认密码区域
    addInputLabel(layout, "确认密码");
    m_confirmContainer = new QWidget();
    m_confirmContainer->setObjectName("PassContainer");
    m_confirmContainer->setFixedHeight(45);
    QHBoxLayout *confirmLayout = new QHBoxLayout(m_confirmContainer);
    confirmLayout->setContentsMargins(10, 0, 5, 0);
    m_confirmPassEdit = new QLineEdit();
    m_confirmPassEdit->setPlaceholderText("再次输入密码");
    m_confirmPassEdit->setEchoMode(QLineEdit::Password);
    m_confirmPassEdit->setFrame(false);
    m_confirmPassEdit->setStyleSheet("background: transparent; border: none; font-size: 15px;");
    m_confirmEyeBtn = new QToolButton();
    m_confirmEyeBtn->setText("👁");
    m_confirmEyeBtn->setCursor(Qt::PointingHandCursor);
    m_confirmEyeBtn->setFixedSize(40, 40);
    m_confirmEyeBtn->setStyleSheet("border: none; background: transparent; color: #95a5a6; font-size: 16px;");
    connect(m_confirmEyeBtn, &QToolButton::clicked, this, &RegisterWidget::toggleConfirmPassVisibility);
    confirmLayout->addWidget(m_confirmPassEdit);
    confirmLayout->addWidget(m_confirmEyeBtn);
    layout->addWidget(m_confirmContainer); // 【关键】加入布局
    layout->addSpacing(5);

    // 5. 角色区域
    addInputLabel(layout, "注册身份");
    m_roleCombo = new QComboBox();
    m_roleCombo->addItem("我是学生", "student");
    m_roleCombo->addItem("我是管理员", "admin");
    m_roleCombo->setFixedHeight(40);
    connect(m_roleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RegisterWidget::onRoleChanged);
    layout->addWidget(m_roleCombo); // 【关键】加入布局

    m_adminSecretEdit = new QLineEdit();
    m_adminSecretEdit->setPlaceholderText("请输入管理员密钥");
    m_adminSecretEdit->setFixedHeight(45);
    m_adminSecretEdit->setVisible(false);
    m_adminSecretEdit->setObjectName("NormalEdit");
    layout->addWidget(m_adminSecretEdit);
    layout->addSpacing(5);

    // 6. 验证码区域
    addInputLabel(layout, "验证码");
    QHBoxLayout *captchaLayout = new QHBoxLayout();
    m_captchaEdit = new QLineEdit();
    m_captchaEdit->setPlaceholderText("输入右侧字符");
    m_captchaEdit->setFixedHeight(42);
    m_captchaEdit->setMaxLength(4);
    m_captchaEdit->setObjectName("NormalEdit");
    m_captchaLabel = new CaptchaLabel(this);
    captchaLayout->addWidget(m_captchaEdit, 1);
    captchaLayout->addWidget(m_captchaLabel, 0);
    layout->addLayout(captchaLayout); // 【关键】加入布局

    layout->addSpacing(20);

    // 7. 注册按钮和消息
    m_regBtn = new QPushButton("立即注册");
    m_regBtn->setFixedHeight(50);
    m_regBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_regBtn);

    m_msgLabel = new QLabel("");
    m_msgLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_msgLabel);

    // 底部填充，把内容顶上去
    layout->addStretch();

    // 焦点顺序设置 (保持不变)
    connect(m_userEdit, &QLineEdit::returnPressed, m_passEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_passEdit, &QLineEdit::returnPressed, m_confirmPassEdit, QOverload<>::of(&QLineEdit::setFocus));
    connect(m_confirmPassEdit, &QLineEdit::returnPressed, this, [this](){
        if(m_adminSecretEdit->isVisible()) m_adminSecretEdit->setFocus();
        else m_captchaEdit->setFocus();
    });
    connect(m_captchaEdit, &QLineEdit::returnPressed, this, &RegisterWidget::onRegisterClicked);
    connect(m_regBtn, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
}
// ==========================================
//  AJAX 核心逻辑 (防抖 + 检查)
// ==========================================

// 1. 监听输入变化
void RegisterWidget::onUserTextChanged(const QString &text) {
    // 每次输入，先重置状态
    m_isUserValid = false;

    // 如果为空，清空提示
    if(text.trimmed().isEmpty()) {
        m_userStatusLabel->clear();
        m_debounceTimer->stop();
        return;
    }

    // 简单的前端校验
    if(text.length() < 3) {
        m_userStatusLabel->setText("长度不够");
        m_userStatusLabel->setStyleSheet("color: #7f8c8d;"); // 灰色
        m_debounceTimer->stop(); // 不发送请求
        return;
    }

    // 显示 "正在检查..." 并启动/重置定时器
    m_userStatusLabel->setText("正在检查...");
    m_userStatusLabel->setStyleSheet("color: #3498db;"); // 蓝色
    m_debounceTimer->start(); // 重新开始计时 500ms
}

// 2. 定时器结束，发送请求
void RegisterWidget::sendCheckUserRequest() {
    QString user = m_userEdit->text().trimmed();
    if(user.length() < 3) return;

    QJsonObject req;
    req["type"] = "check_username"; // 协议头
    req["username"] = user;

    bool sent = NetworkManager::instance().sendRequest(req);
    if (!sent) {
        // 如果发送失败（例如未连接）
        m_userStatusLabel->setText("无法连接检测");
        m_userStatusLabel->setStyleSheet("color: #e67e22;"); // 橙色

        // 尝试触发重连(静默)
        if(!m_serverIp.isEmpty())
            NetworkManager::instance().connectToHost(m_serverIp, m_serverPort);
    }
}

// 3. 接收服务器结果 (槽函数)
void RegisterWidget::onUserCheckResult(bool isExist, const QString &msg) {
    // 再次确认当前输入框内容（防止网络延迟导致结果错位）
    // 理想情况下服务器应该返回 check_username 以及它是针对哪个 username 的
    // 这里做简单处理：

    if (isExist) {
        // 存在 = 不可用
        m_userStatusLabel->setText("❌ 已被占用");
        m_userStatusLabel->setStyleSheet("color: #c0392b;"); // 红色
        m_isUserValid = false;
    } else {
        // 不存在 = 可用
        m_userStatusLabel->setText("✔ 用户名可用");
        m_userStatusLabel->setStyleSheet("color: #27ae60;"); // 绿色
        m_isUserValid = true;
    }

    // 如果有服务器的具体消息，可以覆盖
    if(!msg.isEmpty()) {
        // m_userStatusLabel->setText(msg);
    }
}

// ==========================================
//  其他辅助逻辑
// ==========================================

void RegisterWidget::togglePassVisibility() {
    if(m_passEdit->echoMode() == QLineEdit::Password) {
        m_passEdit->setEchoMode(QLineEdit::Normal);
        m_passEyeBtn->setText("🚫");
    } else {
        m_passEdit->setEchoMode(QLineEdit::Password);
        m_passEyeBtn->setText("👁");
    }
    m_passEdit->setFocus();
}

void RegisterWidget::toggleConfirmPassVisibility() {
    if(m_confirmPassEdit->echoMode() == QLineEdit::Password) {
        m_confirmPassEdit->setEchoMode(QLineEdit::Normal);
        m_confirmEyeBtn->setText("🚫");
    } else {
        m_confirmPassEdit->setEchoMode(QLineEdit::Password);
        m_confirmEyeBtn->setText("👁");
    }
    m_confirmPassEdit->setFocus();
}

void RegisterWidget::setupStyle() {
    // 更新了样式，增加了 #FormLabel 的样式
    QString qss = R"(
        #RegLeftPanel { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4e4376, stop:1 #2b5876); }
        #RegRightPanel { background-color: #ffffff; }
        #RegHeader { font-size: 32px; color: #2c3e50; font-weight: bold; font-family: "Microsoft YaHei"; }
        #RegSubText { font-size: 14px; color: #7f8c8d; margin-bottom: 2px; }

        /* 新增：表单标题样式 */
        #FormLabel { font-size: 14px; color: #34495e; font-weight: bold; }

        #LinkBtn { background: transparent; color: #3498db; border: none; font-size: 14px; font-weight: bold; text-align: left; padding: 0; }
        #LinkBtn:hover { text-decoration: underline; color: #2980b9; }
        #NormalEdit { border: 2px solid #f0f2f5; border-radius: 8px; background: #fcfcfc; font-size: 15px; padding: 0 10px; }
        #NormalEdit:focus { border: 2px solid #3498db; background: #ffffff; }
        #PassContainer { border: 2px solid #f0f2f5; border-radius: 8px; background: #fcfcfc; }
        #PassContainer:hover { border: 2px solid #bdc3c7; }
        QComboBox { border: 2px solid #f0f2f5; border-radius: 8px; padding: 5px 10px; background: #fcfcfc; font-size: 14px; }
        QComboBox:focus { border: 2px solid #3498db; }
        QComboBox::drop-down { border: none; width: 25px; }
        QPushButton { border-radius: 8px; font-family: "Microsoft YaHei"; }
        QPushButton:!flat { background-color: #3498db; color: white; font-size: 18px; font-weight: bold; border: none; }
        QPushButton:!flat:hover { background-color: #2980b9; }
        QPushButton:!flat:pressed { background-color: #1f618d; }
        QPushButton:disabled { background-color: #bdc3c7; color: #ffffff; }
        CaptchaLabel { border: 1px solid #eee; border-radius: 6px; }
    )";
    this->setStyleSheet(qss);
}

void RegisterWidget::onRoleChanged(int index) {
    QString role = m_roleCombo->itemData(index).toString();
    if(role == "admin") {
        m_adminSecretEdit->setVisible(true);
    } else {
        m_adminSecretEdit->setVisible(false);
        m_adminSecretEdit->clear();
    }
}

void RegisterWidget::mousePressEvent(QMouseEvent *event) {
    setFocus();
    QWidget::mousePressEvent(event);
}

void RegisterWidget::onRegisterClicked() {
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text();
    QString confirm = m_confirmPassEdit->text();
    QString role = m_roleCombo->currentData().toString();
    QString captchaInput = m_captchaEdit->text().trimmed();
    QString captchaCorrect = m_captchaLabel->getCode();

    // 1. 校验验证码
    if (captchaInput.compare(captchaCorrect, Qt::CaseInsensitive) != 0) {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("验证码错误");
        m_captchaLabel->regenerate();
        m_captchaEdit->clear();
        return;
    }

    // 2. [新增] 校验用户名是否已确认可用
    // 如果用户输入很快然后马上点注册，可能状态还没回来，所以这里要判断一下
    if (!m_isUserValid) {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("请检查用户名是否可用");
        return;
    }

    // 3. 常规校验
    if(user.length() < 3) {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("用户名长度不能少于3位");
        return;
    }
    if(pass.length() < 6) {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("密码长度不能少于6位");
        return;
    }
    if(pass != confirm) {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("两次输入的密码不一致");
        return;
    }
    if (role == "admin" && m_adminSecretEdit->text() != "admin123") {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("管理员密钥错误");
        return;
    }

    // 4. 发送注册请求
    m_regBtn->setText("正在连接...");
    m_regBtn->setEnabled(false);
    m_msgLabel->setText("");

    QJsonObject req;
    req["type"] = "register";
    req["data"] = QJsonObject{
        {"username", user},
        {"password", pass},
        {"role", role}
    };

    if(NetworkManager::instance().sendRequest(req)) {
        m_regBtn->setText("注册中...");
        return;
    }

    // 断线重连逻辑
    if(!m_serverIp.isEmpty() && m_serverPort > 0) {
        m_msgLabel->setStyleSheet("color: #e67e22;");
        m_msgLabel->setText("连接断开，正在尝试重连...");
        NetworkManager::instance().connectToHost(m_serverIp, m_serverPort);
        QTimer::singleShot(1500, this, [this, req](){
            if(NetworkManager::instance().sendRequest(req)) {
                m_msgLabel->setText("");
                m_regBtn->setText("注册中...");
            } else {
                m_msgLabel->setStyleSheet("color: #c0392b;");
                m_msgLabel->setText("无法连接服务器");
                m_regBtn->setText("立即注册");
                m_regBtn->setEnabled(true);
            }
        });
    } else {
        m_msgLabel->setStyleSheet("color: #c0392b;");
        m_msgLabel->setText("网络错误");
        m_regBtn->setText("立即注册");
        m_regBtn->setEnabled(true);
    }
}
