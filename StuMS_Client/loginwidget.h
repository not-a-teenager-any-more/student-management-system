#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QAction>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QToolButton>
#include "registerwidget.h"

// --- 图片字符验证码类 ---
class ImageCaptcha : public QWidget
{
    Q_OBJECT
public:
    explicit ImageCaptcha(QWidget *parent = nullptr);
    QString getCode() const { return m_code; }
    void regenerate();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_code;
    QList<QColor> m_colors;
};

// --- 登录窗口主类 ---
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void loginSuccess(QString role, QString username);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    // [新增] 窗口显示事件：用于自动重置界面状态
    void showEvent(QShowEvent *event) override;

private slots:
    void onLoginClicked();
    void toggleServerSettings();
    void togglePasswordVisibility();
    void onRoleChanged(int id);
    void onLoginTimeout();

private:
    void setupUi();
    void setupLeftPanel();
    void setupRightPanel();
    void setupStyle();

    // 错误处理核心函数
    void handleLoginFailed(const QString &msg);

    QWidget *m_leftPanel;
    QWidget *m_rightPanel;

    QWidget *m_roleContainer;
    QButtonGroup *m_roleGroup;
    QPushButton *m_btnStudent;
    QPushButton *m_btnAdmin;

    QLineEdit *m_userEdit;

    // 密码组合
    QWidget *m_passContainer;
    QLineEdit *m_passEdit;
    QToolButton *m_eyeBtn;

    // 验证码
    ImageCaptcha *m_captchaImg;
    QLineEdit *m_captchaEdit;

    QPushButton *m_loginBtn;
    QLabel *m_msgLabel;

    QWidget *m_settingsContainer;
    QLineEdit *m_ipEdit;
    QLineEdit *m_portEdit;
    QPushButton *m_settingsBtn;
    bool m_isSettingsVisible = false;

    QPushButton *m_goToRegBtn;
    RegisterWidget *m_regWidget = nullptr;
    QTimer *m_timeoutTimer;
};

#endif // LOGINWIDGET_H
