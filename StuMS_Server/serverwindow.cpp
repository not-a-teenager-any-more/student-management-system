#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QMessageBox>    // 消息提示框
#include <QJsonDocument>  // JSON文档处理
#include <QJsonObject>    // JSON对象
#include <QJsonArray>     // JSON数组
#include <QSqlQuery>      // SQL查询
#include <QSqlError>      // SQL错误处理
#include <QDateTime>      // 日期时间
#include <QHostAddress>   // IP地址处理
#include <QThread>        // 线程支持

// 构造函数
ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)  // 创建UI对象
{
    ui->setupUi(this);  // 设置UI

    // 创建测试套接字并设置属性（模拟已登录的管理员）
    m_testSocket = new QTcpSocket(this);
    m_testSocket->setProperty("username", "testuser");
    m_testSocket->setProperty("role", "admin");
    m_testSocket->setProperty("authenticated", true);

    // 初始化UI
    setWindowTitle("学生信息管理系统 - 服务器");
    ui->logTextEdit->setReadOnly(true);  // 日志区域设为只读
    ui->portSpinBox->setValue(12345);    // 默认端口

    // 连接按钮信号槽
    connect(ui->startButton, &QPushButton::clicked, this, [this](){
        if (m_server && m_server->isListening()) {
            // 停止服务器
            m_server->close();
            ui->startButton->setText("启动服务器");
            logMessage("服务器已停止");
        } else {
            // 启动服务器
            setupServer();
        }
    });

    // 清除日志按钮
    connect(ui->clearLogButton, &QPushButton::clicked, ui->logTextEdit, &QTextEdit::clear);
    // 发送测试请求按钮
    connect(ui->sendTestButton, &QPushButton::clicked, this, &ServerWindow::onSendTestRequest);

    // 初始化数据库
    if (!initDatabase()) {
        QMessageBox::critical(this, "错误", "无法初始化数据库!");
    }
}

// 析构函数
ServerWindow::~ServerWindow()
{
    closeAllConnections();  // 关闭所有客户端连接
    if (m_server) {
        m_server->close();   // 关闭服务器
        delete m_server;     // 释放服务器内存
    }
    m_db.close();            // 关闭数据库连接
    delete ui;               // 释放UI内存
}

// 初始化数据库
bool ServerWindow::initDatabase()
{
    // 创建SQLite数据库连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("student_management.db");  // 数据库文件名

    // 尝试打开数据库
    if (!m_db.open()) {
        logMessage("无法打开数据库: " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query;
    bool success = true;

    // 1. 创建用户表 (用于登录和注册)
    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "role TEXT NOT NULL CHECK(role IN ('admin', 'student')))"
        );

    // 2. 创建学生表
    success &= query.exec(
        "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "student_id TEXT NOT NULL UNIQUE,"
        "name TEXT NOT NULL,"
        "gender TEXT NOT NULL CHECK(gender IN ('male', 'female')),"
        "age INTEGER NOT NULL,"
        "department TEXT NOT NULL,"
        "major TEXT NOT NULL,"
        "class TEXT NOT NULL,"
        "phone TEXT,"
        "email TEXT)"
        );

    // 创建索引（加速查询）
    success &= query.exec("CREATE INDEX IF NOT EXISTS idx_student_id ON students(student_id)");
    success &= query.exec("CREATE INDEX IF NOT EXISTS idx_student_name ON students(name)");

    // 添加默认管理员账户（用户名：admin，密码：123456）
    // 注意：注册功能依赖于 users 表存在
    query.prepare("INSERT OR IGNORE INTO users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue("admin");
    query.addBindValue("123456");
    query.addBindValue("admin");
    success &= query.exec();

    // 检查数据库初始化是否成功
    if (!success) {
        logMessage("数据库初始化失败: " + query.lastError().text());
        return false;
    }

    logMessage("数据库初始化成功");
    return true;
}

// 设置并启动服务器
void ServerWindow::setupServer()
{
    // 清理现有服务器
    if (m_server) {
        m_server->close();
        delete m_server;
    }

    // 创建新的TCP服务器
    m_server = new QTcpServer(this);
    // 连接新连接信号
    connect(m_server, &QTcpServer::newConnection, this, &ServerWindow::onNewConnection);

    // 获取端口号并启动监听
    quint16 port = static_cast<quint16>(ui->portSpinBox->value());
    if (!m_server->listen(QHostAddress::Any, port)) {
        logMessage("无法启动服务器: " + m_server->errorString());
        return;
    }

    // 更新UI状态
    ui->startButton->setText("停止服务器");
    logMessage(QString("服务器已启动，监听端口 %1").arg(port));
}

// 处理新客户端连接
void ServerWindow::onNewConnection()
{
    // 处理所有等待连接的客户端
    while (m_server->hasPendingConnections()) {
        // 获取新连接的套接字
        QTcpSocket *client = m_server->nextPendingConnection();
        m_clients.append(client);  // 添加到客户端列表

        // 连接信号槽
        connect(client, &QTcpSocket::readyRead, this, &ServerWindow::onReadyRead);
        connect(client, &QTcpSocket::disconnected, this, &ServerWindow::onClientDisconnected);

        // 记录连接信息
        QString clientInfo = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
        logMessage(QString("新的客户端连接: %1").arg(clientInfo));
    }
}

// 客户端断开连接处理
void ServerWindow::onClientDisconnected()
{
    // 获取断开连接的客户端
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    // 记录断开信息
    QString clientInfo = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
    logMessage(QString("客户端断开连接: %1").arg(clientInfo));

    // 从列表中移除并删除对象
    m_clients.removeOne(client);
    client->deleteLater();
}

// 读取客户端数据 (解决TCP粘包)
void ServerWindow::onReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QString clientInfo = QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort());
    QByteArray buffer = client->readAll();

    while (!buffer.isEmpty()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(buffer, &error);

        // 情况1: 完美的单个包
        if (error.error == QJsonParseError::NoError) {
            logRequest(clientInfo, buffer);
            QByteArray response = processRequest(client, buffer);
            if (!response.isEmpty()) {
                client->write(response);
                logResponse(clientInfo, response);
            }
            buffer.clear();
        }
        // 情况2: 发生粘包
        else if (error.error == QJsonParseError::GarbageAtEnd) {
            QByteArray validRequest = buffer.left(error.offset);
            QByteArray remainingData = buffer.mid(error.offset);

            logRequest(clientInfo, validRequest);
            QByteArray response = processRequest(client, validRequest);
            if (!response.isEmpty()) {
                client->write(response);
                logResponse(clientInfo, response);
            }
            buffer = remainingData.trimmed();
        }
        // 情况3: 其他错误 (简单处理: 尝试解析或清空)
        else {
            // 这里简单处理：如果不是半包（数据没收全），就尝试解析或报错
            // 在生产环境中应处理 UnterminatedObject 等情况
            logRequest(clientInfo, buffer);
            QByteArray response = processRequest(client, buffer);
            if (!response.isEmpty()) {
                client->write(response);
                logResponse(clientInfo, response);
            }
            buffer.clear();
        }
    }
}

// 处理请求（核心方法）
QByteArray ServerWindow::processRequest(QTcpSocket *client, const QByteArray &request)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request, &error);

    if (error.error != QJsonParseError::NoError) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"success", false}, {"message", "无效的JSON格式: " + error.errorString()}
                                          }).toJson();
    }

    if (!doc.isObject()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"success", false}, {"message", "请求必须是JSON对象"}
                                          }).toJson();
    }

    QJsonObject requestObj = doc.object();
    QString type = requestObj.value("type").toString();
    QJsonObject data = requestObj.value("data").toObject();

    // ==========================================
    // 【修改点】 路由分发逻辑
    // ==========================================
    if (type == "login") {
        return handleLogin(client, data);
    } else if (type == "register") {
        return handleRegister(data);
    } else if (type == "check_username") {
        // [新增] 路由到检查用户名函数
        // 注意：客户端 NetworkManager 发送时 username 是在顶层的，不是在 data 里的
        // 所以我们把整个 requestObj 传过去
        return handleCheckUsername(requestObj);
    } else if (type == "add_student") {
        return handleAddStudent(data);
    } else if (type == "delete_student") {
        return handleDeleteStudent(data);
    } else if (type == "delete_students") {
        return handleDeleteStudents(data);
    } else if (type == "update_student") {
        return handleUpdateStudent(data);
    } else if (type == "query_students") {
        return handleQueryStudents(data);
    } else if (type == "export_data") {
        return handleExportData(data);
    } else {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"success", false}, {"message", "未知的请求类型: " + type}
                                          }).toJson();
    }
}

// ==========================================
// 【新增】 实现 handleCheckUsername
// ==========================================
QByteArray ServerWindow::handleCheckUsername(const QJsonObject &req)
{
    // 获取用户名 (从顶层获取)
    QString username = req.value("username").toString().trimmed();

    // 查询数据库
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE username = ?");
    query.addBindValue(username);

    bool exists = false;
    if (query.exec() && query.next()) {
        exists = true;
    }

    // 构造回复 (必须包含 type: check_username_result)
    // 客户端 NetworkManager 正是靠这个 type 来识别并更新界面的
    QJsonObject resp;
    resp["type"] = "check_username_result";
    resp["exists"] = exists;
    resp["message"] = exists ? "用户名已存在" : "用户名可用";

    return QJsonDocument(resp).toJson(QJsonDocument::Compact);
}

// 处理登录请求
QByteArray ServerWindow::handleLogin(QTcpSocket *client, const QJsonObject &data)
{
    QString username = data.value("username").toString();
    QString password = data.value("password").toString();
    QString role = data.value("role").toString();

    QSqlQuery query;
    query.prepare("SELECT password, role FROM users WHERE username = ? AND role = ?");
    query.addBindValue(username);
    query.addBindValue(role);

    if (!query.exec() || !query.next()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"success", false}, {"message", "用户名或角色错误"}
                                          }).toJson();
    }

    QString storedPassword = query.value(0).toString();

    if (password != storedPassword) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"success", false}, {"message", "密码错误"}
                                          }).toJson();
    }

    client->setProperty("username", username);
    client->setProperty("role", role);
    client->setProperty("authenticated", true);

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "login"}, {"success", true}, {"message", "登录成功"},
                                          {"data", QVariantMap{{"role", role}}}
                                      }).toJson();
}

// 处理注册请求
QByteArray ServerWindow::handleRegister(const QJsonObject &data)
{
    QString username = data.value("username").toString().trimmed();
    QString password = data.value("password").toString();
    QString role = data.value("role").toString();

    if (username.isEmpty() || password.isEmpty()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "register"}, {"success", false}, {"message", "用户名或密码不能为空"}
                                          }).toJson();
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT username FROM users WHERE username = ?");
    checkQuery.addBindValue(username);
    if (checkQuery.exec() && checkQuery.next()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "register"}, {"success", false}, {"message", "用户名已存在"}
                                          }).toJson();
    }

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    insertQuery.addBindValue(username);
    insertQuery.addBindValue(password);
    insertQuery.addBindValue(role);

    if (!insertQuery.exec()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "register"}, {"success", false}, {"message", "注册数据库错误"}
                                          }).toJson();
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "register"}, {"success", true}, {"message", "注册成功"}
                                      }).toJson();
}

// 处理添加学生请求
QByteArray ServerWindow::handleAddStudent(const QJsonObject &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO students (student_id, name, gender, age, department, major, class, phone, email) "
                  "VALUES (:student_id, :name, :gender, :age, :department, :major, :class, :phone, :email)");

    query.bindValue(":student_id", data.value("student_id").toString());
    query.bindValue(":name", data.value("name").toString());
    query.bindValue(":gender", data.value("gender").toString());
    query.bindValue(":age", data.value("age").toInt());
    query.bindValue(":department", data.value("department").toString());
    query.bindValue(":major", data.value("major").toString());
    query.bindValue(":class", data.value("class").toString());
    query.bindValue(":phone", data.value("phone").toString());
    query.bindValue(":email", data.value("email").toString());

    if (!query.exec()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "add_student"}, {"success", false}, {"message", "添加学生失败: " + query.lastError().text()}
                                          }).toJson();
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "add_student"}, {"success", true}, {"message", "学生添加成功"}
                                      }).toJson();
}

// 处理删除单个学生请求
QByteArray ServerWindow::handleDeleteStudent(const QJsonObject &data)
{
    QString studentId = data.value("student_id").toString();
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE student_id = ?");
    query.addBindValue(studentId);

    if (!query.exec()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "delete_student"}, {"success", false}, {"message", "删除失败: " + query.lastError().text()}
                                          }).toJson();
    }

    if (query.numRowsAffected() == 0) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "delete_student"}, {"success", false}, {"message", "未找到该学生"}
                                          }).toJson();
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "delete_student"}, {"success", true}, {"message", "学生删除成功"}
                                      }).toJson();
}

// 处理批量删除学生请求
QByteArray ServerWindow::handleDeleteStudents(const QJsonObject &data)
{
    QJsonArray studentIds = data.value("student_ids").toArray();
    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE student_id = ?");

    int deletedCount = 0;
    for (const QJsonValue &idValue : studentIds) {
        query.addBindValue(idValue.toString());
        if (query.exec()) deletedCount += query.numRowsAffected();
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "delete_students"}, {"success", true}, {"message", QString("成功删除 %1 个学生").arg(deletedCount)}
                                      }).toJson();
}

// 处理更新学生请求
QByteArray ServerWindow::handleUpdateStudent(const QJsonObject &data)
{
    QString studentId = data.value("student_id").toString();
    QSqlQuery query;
    query.prepare("UPDATE students SET name=:name, gender=:gender, age=:age, department=:department, "
                  "major=:major, class=:class, phone=:phone, email=:email WHERE student_id=:student_id");

    query.bindValue(":student_id", studentId);
    query.bindValue(":name", data.value("name").toString());
    query.bindValue(":gender", data.value("gender").toString());
    query.bindValue(":age", data.value("age").toInt());
    query.bindValue(":department", data.value("department").toString());
    query.bindValue(":major", data.value("major").toString());
    query.bindValue(":class", data.value("class").toString());
    query.bindValue(":phone", data.value("phone").toString());
    query.bindValue(":email", data.value("email").toString());

    if (!query.exec()) {
        return QJsonDocument::fromVariant(QVariantMap{
                                              {"type", "update_student"}, {"success", false}, {"message", "更新失败: " + query.lastError().text()}
                                          }).toJson();
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "update_student"}, {"success", true}, {"message", "更新成功"}
                                      }).toJson();
}

// 处理查询学生请求
QByteArray ServerWindow::handleQueryStudents(const QJsonObject &data)
{
    QString keyword = data.value("keyword").toString();
    int page = data.value("page").toInt(1);
    int pageSize = data.value("pageSize").toInt(100);
    QString role = data.value("role").toString();
    QString currentUser = data.value("username").toString();

    QSqlQuery query;
    QString queryStr = "SELECT * FROM students";
    QVariantList bindValues;
    QStringList conditions;

    if (!keyword.isEmpty()) {
        conditions.append("(name LIKE ? OR student_id LIKE ?)");
        bindValues << "%" + keyword + "%" << "%" + keyword + "%";
    }
    if (role == "student") {
        conditions.append("student_id = ?");
        bindValues << currentUser;
    }

    if (!conditions.isEmpty()) queryStr += " WHERE " + conditions.join(" AND ");
    queryStr += " LIMIT ? OFFSET ?";
    bindValues << pageSize << (page - 1) * pageSize;

    query.prepare(queryStr);
    for (const auto &val : bindValues) query.addBindValue(val);

    if (!query.exec()) {
        return QJsonDocument::fromVariant(QVariantMap{{"success", false}, {"message", "查询失败"}}).toJson();
    }

    QJsonArray studentsArray;
    while (query.next()) {
        QJsonObject studentObj;
        studentObj["student_id"] = query.value("student_id").toString();
        studentObj["name"] = query.value("name").toString();
        studentObj["gender"] = query.value("gender").toString();
        studentObj["age"] = query.value("age").toInt();
        studentObj["department"] = query.value("department").toString();
        studentObj["major"] = query.value("major").toString();
        studentObj["class"] = query.value("class").toString();
        studentObj["phone"] = query.value("phone").toString();
        studentObj["email"] = query.value("email").toString();
        studentsArray.append(studentObj);
    }

    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "query_students"}, {"success", true}, {"message", "查询成功"},
                                          {"data", QVariantMap{{"students", studentsArray}}}
                                      }).toJson();
}

// 处理导出数据请求
QByteArray ServerWindow::handleExportData(const QJsonObject &data)
{
    return QJsonDocument::fromVariant(QVariantMap{
                                          {"type", "export_data"}, {"success", true}, {"message", "导出准备完成"}
                                      }).toJson();
}

// 记录日志相关函数
void ServerWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] %2").arg(timestamp, message));
}

void ServerWindow::logRequest(const QString &clientInfo, const QByteArray &request)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] 请求来自 %2:\n%3").arg(timestamp, clientInfo, QString::fromUtf8(request)));
}

void ServerWindow::logResponse(const QString &clientInfo, const QByteArray &response)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] 响应给 %2:\n%3").arg(timestamp, clientInfo, QString::fromUtf8(response)));
}

void ServerWindow::onSendTestRequest()
{
    QString requestText = ui->testRequestEdit->toPlainText().trimmed();
    if (requestText.isEmpty()) return;
    QByteArray requestData = requestText.toUtf8();
    logMessage("发送测试请求:\n" + requestText);
    QByteArray response = processRequest(m_testSocket, requestData);
    ui->logTextEdit->append("测试响应:\n" + QString::fromUtf8(response));
}

void ServerWindow::closeAllConnections()
{
    foreach (QTcpSocket *client, m_clients) {
        client->disconnectFromHost();
        client->deleteLater();
    }
    m_clients.clear();
}
