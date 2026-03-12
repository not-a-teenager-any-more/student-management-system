#include "networkmanager.h"
#include <QDebug>
#include <QJsonParseError>

NetworkManager& NetworkManager::instance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager() {
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, [this](){
        qDebug() << "Server connected!";
        m_buffer.clear();
        emit connected();
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkManager::onErrorOccurred);

    connect(m_socket, &QTcpSocket::disconnected, this, [this](){
        qDebug() << "Server disconnected";
        m_buffer.clear();
        emit disconnected();
    });
}

void NetworkManager::connectToHost(const QString &ip, quint16 port) {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        if (m_currentIp == ip && m_currentPort == port) return;
        m_socket->disconnectFromHost(); // 如果IP变了，先断开
    }
    // 如果正在连接中，也不要重复操作
    if (m_socket->state() == QAbstractSocket::ConnectingState) return;

    m_currentIp = ip;
    m_currentPort = port;
    m_buffer.clear();
    m_socket->connectToHost(ip, port);
}

void NetworkManager::fetchStudents() {
    QJsonObject req;
    req["type"] = "query_students";
    req["limit"] = 100000;
    req["page"] = 1;
    qDebug() << "Fetching all students data...";
    sendRequest(req);
}

// [新增] 发送检查用户名请求
void NetworkManager::checkUsername(const QString &username) {
    QJsonObject req;
    req["type"] = "check_username";
    req["username"] = username;
    sendRequest(req);
}

bool NetworkManager::sendRequest(const QJsonObject &req) {
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Send failed: Not connected";
        // 模拟本地网络错误反馈，防止界面卡死
        QString type = req["type"].toString();
        if (type == "login") emit loginResult(false, "", "网络未连接");
        else if (type == "register") emit registrationResult(false, "网络未连接");
        else if (type == "check_username") emit checkUserResult(false, "无法连接服务器"); // 这里的false/true含义需根据业务约定，这里暂定无法连接不显示占用

        return false;
    }

    // 建议：如果可以修改服务器，最好在JSON前加4字节长度头。
    // 这里保持纯JSON流的方式。
    QByteArray data = QJsonDocument(req).toJson(QJsonDocument::Compact);
    m_socket->write(data);
    return true;
}

bool NetworkManager::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    QString msg = m_socket->errorString();
    if (socketError == QAbstractSocket::ConnectionRefusedError) msg = "无法连接服务器(Connection Refused)";
    else if (socketError == QAbstractSocket::RemoteHostClosedError) msg = "服务器断开连接";
    else if (socketError == QAbstractSocket::HostNotFoundError) msg = "找不到服务器主机";

    qDebug() << "Socket Error:" << msg;
    emit connectionError(msg);
}

// =========================================================
// 【核心修改】：标准的 TCP 粘包/拆包处理逻辑
// =========================================================
// =========================================================
// 【核心修改】：修正后的 TCP 粘包/拆包处理逻辑
// =========================================================
// 请替换 NetworkManager.cpp 中的 onReadyRead 函数
// 文件：networkmanager.cpp
// 用这个函数完全替换原来的 onReadyRead

void NetworkManager::onReadyRead() {
    // 1. 追加数据
    m_buffer.append(m_socket->readAll());

    // 2. 循环解析
    while (!m_buffer.isEmpty()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(m_buffer, &error);

        if (error.error == QJsonParseError::NoError) {
            // [情况1] 完美解析
            if (doc.isObject()) {
                processJsonObject(doc.object());
            }
            m_buffer.clear();
            break;
        }
        else if (error.error == QJsonParseError::GarbageAtEnd) {
            // [情况2] 粘包：解析成功一部分，后面还有数据
            QByteArray validData = m_buffer.left(error.offset);
            QJsonDocument validDoc = QJsonDocument::fromJson(validData);
            if (validDoc.isObject()) {
                processJsonObject(validDoc.object());
            }
            // 移除已处理部分，继续循环
            m_buffer = m_buffer.mid(error.offset);
        }
        else if (error.error == QJsonParseError::UnterminatedObject ||
                 error.error == QJsonParseError::UnterminatedArray ||
                 error.error == QJsonParseError::UnterminatedString) {
            // [情况3] 拆包（数据没收全）：保留 buffer，等待下次读取
            // 这种错误说明 JSON 还没传完，比如只收到了 "{"id": 1
            break;
        }
        else {
            // [情况4] 数据严重损坏：清空防止死循环
            qDebug() << "JSON Error:" << error.errorString();
            m_buffer.clear();
            break;
        }
    }
}
// 业务逻辑分发
void NetworkManager::processJsonObject(const QJsonObject &root) {
    QString type = root["type"].toString();
    // 兼容部分后端返回 success 字段或者 status 字段
    bool success = root.contains("success") ? root["success"].toBool() : true;
    QString msg = root["message"].toString();
    QJsonObject dataObj = root["data"].toObject();

    // qDebug() << "Processing Msg Type:" << type;

    if(type == "login") {
        emit loginResult(success, dataObj["role"].toString(), msg);
    }
    else if(type == "query_students") {
        QJsonArray arr = dataObj["students"].toArray();
        emit studentsDataReceived(arr);
    }
    else if (type == "register") {
        emit registrationResult(success, msg);
    }
    // [新增] 处理用户名检查结果
    // 假设后端返回: { "type": "check_username_result", "exists": true, "message": "已占用" }
    else if (type == "check_username_result") {
        // 这里的字段名取决于你后端怎么写的
        bool exists = root["exists"].toBool();
        emit checkUserResult(exists, msg);
    }
    else {
        // 其他 CRUD 操作
        emit operationResult(success, msg);
    }
}
