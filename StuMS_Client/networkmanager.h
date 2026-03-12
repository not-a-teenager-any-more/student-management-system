#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    static NetworkManager& instance();

    // 连接服务器
    void connectToHost(const QString &ip, quint16 port);
    // 发送通用请求
    bool sendRequest(const QJsonObject &req);
    // 检查连接状态
    bool isConnected() const;

    // --- 业务功能 ---
    void fetchStudents(); // 获取学生列表

    // [新增] 检查用户名是否存在
    void checkUsername(const QString &username);

private:
    NetworkManager();
    // 禁止拷贝
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    QTcpSocket *m_socket;
    QString m_currentIp;
    quint16 m_currentPort;

    // 数据缓存区
    QByteArray m_buffer;

    // 内部处理单个JSON对象的函数
    void processJsonObject(const QJsonObject &root);

signals:
    void connected();
    void disconnected();
    void connectionError(const QString &msg);

    // 登录结果
    void loginResult(bool success, const QString &role, const QString &msg);
    // 学生数据结果
    void studentsDataReceived(const QJsonArray &data);
    // 注册结果
    void registrationResult(bool success, const QString &msg);
    // 通用操作结果
    void operationResult(bool success, const QString &msg);

    // [新增] 用户名检查结果 (exists: true表示被占用)
    void checkUserResult(bool exists, const QString &msg);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
};

#endif // NETWORKMANAGER_H
