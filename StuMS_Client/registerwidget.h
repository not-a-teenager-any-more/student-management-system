#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QCloseEvent>
#include <QToolButton>
#include <QTimer>      // [关键] 用于防抖延时
#include <QHBoxLayout> // [关键] 用于布局
#include <QVBoxLayout>

// --- 验证码标签类 (保持不变) ---
class CaptchaLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CaptchaLabel(QWidget *parent = nullptr);
    QString getCode() const { return m_code; }
    void regenerate();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_code;
    QList<QColor> m_colors;
};
// ----------------------------

class RegisterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    void setServerInfo(const QString &ip, quint16 port);

signals:
    void goBackToLogin();

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    // 按钮逻辑
    void onRegisterClicked();
    void onRoleChanged(int index);
    void togglePassVisibility();
    void toggleConfirmPassVisibility();

    // [新增] AJAX 风格的用户检查逻辑
    void onUserTextChanged(const QString &text); // 监听输入
    void sendCheckUserRequest();                 // 发送网络请求

public slots:
    // [新增] 接收 NetworkManager 的检查结果
    // isExist: true=存在(不可用), false=不存在(可用)
    void onUserCheckResult(bool isExist, const QString &msg = "");

private:
    void setupUi();
    void setupLeftPanel();
    void setupRightPanel();
    void setupStyle();

    // [辅助] 快速添加 "标题 + 状态" 的布局
    QLabel* addInputLabel(QVBoxLayout *layout, const QString &text);

    QString m_serverIp;
    quint16 m_serverPort = 0;

    QWidget *m_leftPanel;
    QWidget *m_rightPanel;

    // --- 用户名相关 ---
    QLineEdit *m_userEdit;
    QLabel *m_userStatusLabel; // 显示 "正在检查..." 或 "✔ 可用"
    QTimer *m_debounceTimer;   // 防抖定时器
    bool m_isUserValid;        // 记录当前用户名是否可用

    // --- 密码框 ---
    QWidget *m_passContainer;
    QLineEdit *m_passEdit;
    QToolButton *m_passEyeBtn;

    QWidget *m_confirmContainer;
    QLineEdit *m_confirmPassEdit;
    QToolButton *m_confirmEyeBtn;

    QComboBox *m_roleCombo;
    QLineEdit *m_adminSecretEdit;

    QLineEdit *m_captchaEdit;
    CaptchaLabel *m_captchaLabel;

    QPushButton *m_regBtn;
    QLabel *m_msgLabel;
};

#endif // REGISTERWIDGET_H
