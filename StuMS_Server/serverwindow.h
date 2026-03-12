#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QTcpServer>   // TCP服务器组件
#include <QTcpSocket>   // TCP套接字组件
#include <QSqlDatabase> // 数据库组件
#include <QThreadPool>  // 线程池管理
#include <QJsonObject>  // JSON对象处理

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr); // 构造函数
    ~ServerWindow();                         // 析构函数

private slots:
    // 服务器事件处理
    void onNewConnection();        // 新客户端连接
    void onClientDisconnected();   // 客户端断开连接
    void onReadyRead();            // 读取客户端数据

    // 测试功能
    void onSendTestRequest();      // 发送测试请求

    // 日志记录
    void logMessage(const QString &message);       // 记录普通消息
    void logRequest(const QString &clientInfo, const QByteArray &request);  // 记录请求
    void logResponse(const QString &clientInfo, const QByteArray &response); // 记录响应

private:
    Ui::ServerWindow *ui;          // UI界面对象
    QTcpServer *m_server = nullptr; // TCP服务器实例
    QTcpSocket *m_testSocket = nullptr; // 测试用的套接字
    QList<QTcpSocket*> m_clients;  // 已连接的客户端列表
    QSqlDatabase m_db;             // 数据库连接
    QThreadPool m_threadPool;      // 线程池（用于异步任务）

    // 核心方法
    bool initDatabase();           // 初始化数据库
    void setupServer();            // 设置并启动服务器
    void closeAllConnections();    // 关闭所有客户端连接

    // 请求处理
    QByteArray processRequest(QTcpSocket *client, const QByteArray &request); // 处理请求

    // 业务逻辑处理函数
    QByteArray handleLogin(QTcpSocket *client, const QJsonObject &data);      // 处理登录
    QByteArray handleRegister(const QJsonObject &data);         // 处理注册请求

    // [新增] 处理用户名检查 (这是解决蓝色字卡住的关键)
    QByteArray handleCheckUsername(const QJsonObject &data);

    QByteArray handleAddStudent(const QJsonObject &data);       // 添加学生
    QByteArray handleDeleteStudent(const QJsonObject &data);    // 删除单个学生
    QByteArray handleDeleteStudents(const QJsonObject &data);   // 批量删除学生
    QByteArray handleUpdateStudent(const QJsonObject &data);    // 更新学生信息
    QByteArray handleQueryStudents(const QJsonObject &data);    // 查询学生
    QByteArray handleExportData(const QJsonObject &data);       // 导出数据
};

#endif // SERVERWINDOW_H
