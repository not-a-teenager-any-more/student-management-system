// #include "clientwindow.h"
// #include "studentdialog.h"
// #include "ui_clientwindow.h"
// #include "exporttaskmodel.h"
// #include "exportworker.h"
// #include <QMessageBox>
// #include <QFileDialog>
// #include <QtCharts/QBarSeries>
// #include <QtCharts/QPieSeries>
// #include <QHostAddress>
// #include <QStandardItemModel>
// #include <QJsonArray>
// #include <QJsonObject>
// #include <QJsonDocument>
// #include <QJsonValue>
// #include <QBarSet>
// #include <QBarCategoryAxis>
// #include <QValueAxis>
// #include <QTimer>
// #include <QStyleFactory>
// #include <QStandardPaths>
// #include <DeleteButtonDelegate.h>

// // 构造函数：初始化客户端窗口
// ClientWindow::ClientWindow(QWidget *parent)
//     : QMainWindow(parent)
//     , m_connecting(false)  // 初始未连接
//     , ui(new Ui::ClientWindow)
//     , m_socket(new QTcpSocket(this))  // 创建TCP套接字
//     , m_authenticated(false)  // 初始未认证
// {
//     ui->setupUi(this);
//     setupUI();  // 初始化UI组件

//     // 初始化导出模型
//     m_exportModel = new ExportTaskModel(this);
//     ui->exportTableView->setModel(m_exportModel);

//     // 设置导出线程池
//     m_exportPool.setMaxThreadCount(3); // 最多3个并发导出

//     // 设置导出路径默认值（文档目录）
//     m_exportPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//     ui->exportPathEdit->setText(m_exportPath);

//     // 设置表格列宽策略
//     ui->exportTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // 序号列
//     ui->exportTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 文件名列自动扩展
//     ui->exportTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // 格式列
//     ui->exportTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // 进度列
//     ui->exportTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // 操作列

//     // 设置删除按钮委托
//     DeleteButtonDelegate *deleteDelegate = new DeleteButtonDelegate(this);
//     ui->exportTableView->setItemDelegateForColumn(4, deleteDelegate);
//     connect(deleteDelegate, &DeleteButtonDelegate::deleteClicked,
//             this, &ClientWindow::onRemoveExportTask);

//     // 连接信号槽：导出相关
//     connect(ui->addToExportBtn, &QPushButton::clicked, this, &ClientWindow::onAddToExportList);
//     connect(ui->exportPathEdit, &QLineEdit::textChanged, this, &ClientWindow::onExportPathChanged);
//     connect(ui->browseExportPathBtn, &QPushButton::clicked, this, &ClientWindow::onBrowseExportPath);
//     connect(ui->exportBtn, &QPushButton::clicked, this, &ClientWindow::onExportAll);

//     // 连接信号槽：学生操作相关
//     connect(ui->loginButton, &QPushButton::clicked, this, &ClientWindow::onLoginButtonClicked);
//     connect(ui->searchButton, &QPushButton::clicked, this, &ClientWindow::onQueryStudents);
//     connect(ui->addButton, &QPushButton::clicked, this, &ClientWindow::onAddStudent);
//     connect(ui->deleteButton, &QPushButton::clicked, this, &ClientWindow::onDeleteStudent);
//     connect(ui->updateButton, &QPushButton::clicked, this, &ClientWindow::onUpdateStudent);
//     connect(ui->themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
//             this, &ClientWindow::onThemeChanged);

//     // 连接信号槽：网络相关
//     connect(m_socket, &QTcpSocket::connected, this, &ClientWindow::onConnected);
//     connect(m_socket, &QTcpSocket::disconnected, this, &ClientWindow::onDisconnected);
//     connect(m_socket, &QTcpSocket::readyRead, this, &ClientWindow::onReadyRead);
//     connect(m_socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
//         showError("连接错误: " + m_socket->errorString());
//     });

//     // 设置默认服务器地址和端口
//     ui->serverAddressEdit->setText("127.0.0.1");
//     ui->serverPortEdit->setText("12345");

//     // 创建定时器定期检查连接状态
//     QTimer *connectionTimer = new QTimer(this);
//     connect(connectionTimer, &QTimer::timeout, this, [this]() {
//         // 如果断开连接且已认证，尝试重新登录
//         if (m_socket->state() == QAbstractSocket::UnconnectedState && m_authenticated) {
//             showError("连接已断开，尝试重新连接...");
//             onLoginButtonClicked();
//         }
//     });
//     connectionTimer->start(5000); // 每5秒检查一次

//     // 应用默认主题（浅色）
//     onThemeChanged(Light);
// }

// // 析构函数：清理资源
// ClientWindow::~ClientWindow()
// {
//     // 断开网络连接
//     if (m_socket->state() == QAbstractSocket::ConnectedState) {
//         m_socket->disconnectFromHost();
//     }
//     delete ui;
// }

// // 初始化UI组件
// void ClientWindow::setupUI()
// {
//     // 设置学生表格模型
//     QStandardItemModel *model = new QStandardItemModel(this);
//     model->setHorizontalHeaderLabels({"学号", "姓名", "性别", "年龄", "院系", "专业", "班级", "电话", "邮箱"});
//     ui->studentsTableView->setModel(model);
//     ui->studentsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 列自适应
//     ui->studentsTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // 扩展选择模式
//     ui->studentsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);  // 按行选择
//     ui->studentsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 禁止编辑

//     // 初始化图表视图
//     m_ageChartView = new QChartView(this);
//     m_genderChartView = new QChartView(this);
//     ui->chartsLayout->addWidget(m_ageChartView);
//     ui->chartsLayout->addWidget(m_genderChartView);

//     // 设置主题选项
//     ui->themeComboBox->addItems({"浅色", "深色", "蓝色"});

//     // 初始状态：主标签页禁用
//     ui->mainTabWidget->setEnabled(false);
//     updatePermissions();  // 更新权限

//     // 设置按钮提示
//     ui->deleteButton->setToolTip("删除选中的学生（可多选）");

//     // 设置按钮图标
//     ui->loginButton->setIcon(QIcon(":/images/login.png"));
//     ui->loginButton->setIconSize(QSize(16, 16));
//     ui->addButton->setIcon(QIcon(":/images/add.png"));
//     ui->addButton->setIconSize(QSize(16, 16));
//     ui->searchButton->setIcon(QIcon(":/images/query.png"));
//     ui->searchButton->setIconSize(QSize(16, 16));
//     ui->deleteButton->setIcon(QIcon(":/images/del.png"));
//     ui->deleteButton->setIconSize(QSize(16, 16));
//     ui->updateButton->setIcon(QIcon(":/images/update.png"));
//     ui->updateButton->setIconSize(QSize(16, 16));
//     ui->addToExportBtn->setIcon(QIcon(":/images/addtoexport.png"));
//     ui->addToExportBtn->setIconSize(QSize(16, 16));
//     ui->browseExportPathBtn->setIcon(QIcon(":/images/folder.png"));
//     ui->browseExportPathBtn->setIconSize(QSize(16, 16));
//     ui->exportBtn->setIcon(QIcon(":/images/export.png"));
//     ui->exportBtn->setIconSize(QSize(16, 16));
// }

// // 处理登录按钮点击
// void ClientWindow::onLoginButtonClicked()
// {
//     // 防止重复连接
//     if (m_connecting || m_socket->state() == QAbstractSocket::ConnectingState) {
//         showError("正在连接服务器，请稍候");
//         return;
//     }

//     // 获取输入参数
//     QString address = ui->serverAddressEdit->text();
//     quint16 port = static_cast<quint16>(ui->serverPortEdit->text().toInt());
//     QString username = ui->usernameEdit->text();
//     QString password = ui->passwordEdit->text();
//     QString role = ui->roleComboBox->currentText() == "管理员" ? "admin" : "student";

//     // 验证输入
//     if (username.isEmpty() || password.isEmpty()) {
//         showError("用户名和密码不能为空");
//         return;
//     }

//     // 构造登录请求（暂存不立即发送）
//     m_pendingRequest = QJsonObject {
//         {"type", "login"},
//         {"data", QJsonObject {
//                      {"username", username},
//                      {"password", password},
//                      {"role", role}
//                  }}
//     };

//     // 开始连接
//     m_connecting = true;
//     ui->loginButton->setEnabled(false);
//     ui->statusLabel->setText("正在连接服务器...");

//     // 连接特定信号
//     connect(m_socket, &QTcpSocket::connected, this, &ClientWindow::handleConnectionEstablished);
//     connect(m_socket, &QAbstractSocket::errorOccurred, this, &ClientWindow::handleConnectionError);

//     // 连接服务器
//     m_socket->connectToHost(address, port);

//     // 设置连接超时（5秒）
//     QTimer::singleShot(5000, this, [this]() {
//         if (m_connecting) {
//             m_socket->abort();
//             handleConnectionError(QAbstractSocket::SocketTimeoutError);
//         }
//     });
// }

// // 处理连接建立
// void ClientWindow::handleConnectionEstablished()
// {
//     m_connecting = false;
//     // 断开临时信号连接
//     disconnect(m_socket, &QTcpSocket::connected, this, &ClientWindow::handleConnectionEstablished);
//     disconnect(m_socket, &QAbstractSocket::errorOccurred, this, &ClientWindow::handleConnectionError);

//     ui->loginButton->setEnabled(true);
//     showMessage("服务器连接成功");

//     // 发送暂存的请求
//     if (!m_pendingRequest.isEmpty()) {
//         sendRequest(m_pendingRequest);
//         m_pendingRequest = QJsonObject(); // 清空暂存
//     }
// }

// // 处理连接错误
// void ClientWindow::handleConnectionError(QAbstractSocket::SocketError error)
// {
//     Q_UNUSED(error);
//     m_connecting = false;
//     // 断开临时信号连接
//     disconnect(m_socket, &QTcpSocket::connected, this, &ClientWindow::handleConnectionEstablished);
//     disconnect(m_socket, &QAbstractSocket::errorOccurred, this, &ClientWindow::handleConnectionError);

//     ui->loginButton->setEnabled(true);
//     showError("连接服务器失败: " + m_socket->errorString());

//     // 尝试自动重连（最多3次）
//     if (m_reconnectAttempts++ < 3) {
//         QTimer::singleShot(1000, this, [this]() {
//             onLoginButtonClicked(); // 1秒后自动重连
//         });
//     } else {
//         m_reconnectAttempts = 0;
//     }
// }

// // 处理连接成功
// void ClientWindow::onConnected()
// {
//     showMessage("已连接到服务器");
// }

// // 处理连接断开
// void ClientWindow::onDisconnected()
// {
//     m_authenticated = false;
//     ui->mainTabWidget->setEnabled(false);
//     showMessage("与服务器的连接已断开");
// }

// // 读取服务器数据
// void ClientWindow::onReadyRead()
// {
//     // 处理所有可用数据
//     while (m_socket->bytesAvailable() > 0) {
//         QByteArray data = m_socket->readAll();
//         QJsonDocument doc = QJsonDocument::fromJson(data);

//         // 验证JSON格式
//         if (doc.isNull() || !doc.isObject()) {
//             showError("无效的服务器响应");
//             return;
//         }

//         QJsonObject response = doc.object();
//         QString type = response.value("type").toString();

//         // 根据响应类型分发处理
//         if (type == "login") {
//             handleLoginResponse(response);
//         } else if (type == "query_students") {
//             handleStudentsData(response);
//         } else if (type == "export_progress") {
//             handleExportProgress(response);
//         } else if (type == "export_complete") {
//             bool success = response.value("data").toObject().value("success").toBool();
//             if (success) {
//                 showMessage("导出完成");
//             } else {
//                 showError("导出失败");
//             }
//         } else {
//             // 通用响应处理
//             bool success = response.value("success").toBool();
//             QString message = response.value("message").toString();
//             if (success) {
//                 showMessage(message);

//                 // 在成功执行增删改操作后自动刷新表格
//                 if (type == "add_student" || type == "update_student" || type == "delete_students") {
//                     onQueryStudents(); // 重新查询数据刷新表格
//                 }
//             } else {
//                 showError(message);
//             }
//         }
//     }
// }

// // 处理登录响应
// void ClientWindow::handleLoginResponse(const QJsonObject &response)
// {
//     bool success = response.value("success").toBool();
//     if (success) {
//         m_authenticated = true;
//         m_username = ui->usernameEdit->text();
//         m_role = response.value("data").toObject().value("role").toString();

//         // 更新UI状态
//         ui->loginGroupBox->setEnabled(false);
//         ui->mainTabWidget->setEnabled(true);
//         ui->statusLabel->setText(QString("已登录 (%1)").arg(m_role == "admin" ? "管理员" : "学生"));

//         updatePermissions();  // 更新权限
//         onQueryStudents(); // 登录成功后自动加载学生数据
//     } else {
//         showError(response.value("message").toString());
//     }
// }

// // 处理学生数据响应
// void ClientWindow::handleStudentsData(const QJsonObject &response)
// {
//     QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->studentsTableView->model());
//     model->removeRows(0, model->rowCount());  // 清空现有数据

//     // 解析学生数据
//     QJsonArray students = response.value("data").toObject().value("students").toArray();
//     for (const QJsonValue &value : students) {
//         QJsonObject student = value.toObject();
//         QList<QStandardItem*> items;
//         auto itemStudentid = new QStandardItem(student.value("student_id").toString());
//         itemStudentid->setEditable(false);
//         items << itemStudentid;
//         items << new QStandardItem(student.value("name").toString());
//         items << new QStandardItem(student.value("gender").toString());
//         items << new QStandardItem(QString::number(student.value("age").toInt()));
//         items << new QStandardItem(student.value("department").toString());
//         items << new QStandardItem(student.value("major").toString());
//         items << new QStandardItem(student.value("class").toString());
//         items << new QStandardItem(student.value("phone").toString());
//         items << new QStandardItem(student.value("email").toString());
//         model->appendRow(items);  // 添加行数据
//     }

//     // 更新统计图表
//     setupCharts();
// }

// // 处理导出进度响应
// void ClientWindow::handleExportProgress(const QJsonObject &response)
// {
//     int progress = response.value("data").toObject().value("progress").toInt();
//     // 预留导出进度处理（当前未实现UI）
//     // ui->exportProgressBar->setValue(progress);
//     // ui->exportStatusLabel->setText(response.value("data").toObject().value("message").toString());
// }

// // 添加学生
// void ClientWindow::onAddStudent()
// {
//     // 创建添加学生对话框
//     StudentDialog dialog(this, false); // false表示添加模式

//     if (dialog.exec() == QDialog::Accepted) {
//         // 构造添加学生请求
//         QJsonObject request {
//             {"type", "add_student"},
//             {"data", QJsonObject {
//                          {"student_id", dialog.studentId()},
//                          {"name", dialog.name()},
//                          {"gender", dialog.gender()},
//                          {"age", dialog.age()},
//                          {"department", dialog.department()},
//                          {"major", dialog.major()},
//                          {"class", dialog.className()},
//                          {"phone", dialog.phone()},
//                          {"email", dialog.email()}
//                      }}
//         };

//         sendRequest(request);  // 发送请求
//     }
// }

// // 删除学生
// void ClientWindow::onDeleteStudent()
// {
//     // 获取选中的行
//     QModelIndexList selectedRows = ui->studentsTableView->selectionModel()->selectedRows();
//     if (selectedRows.isEmpty()) {
//         showError("请先选择要删除的学生");
//         return;
//     }

//     // 收集所有选中的学号
//     QStringList studentIds;

//     //模型-视图
//     QAbstractItemModel *model = ui->studentsTableView->model();

//     for (const QModelIndex &index : selectedRows) {
//         QString studentId = model->index(index.row(), 0).data().toString();
//         studentIds.append(studentId);
//     }

//     // 确认对话框
//     QMessageBox::StandardButton reply;
//     reply = QMessageBox::question(this, "确认删除",
//                                   QString("确定要删除选中的 %1 个学生吗？").arg(studentIds.size()),
//                                   QMessageBox::Yes|QMessageBox::No);
//     if (reply != QMessageBox::Yes) {
//         return;
//     }

//     // 构造删除请求
//     QJsonObject request {
//         {"type", "delete_students"},  // 注意类型改为复数形式
//         {"data", QJsonObject {
//                      {"student_ids", QJsonArray::fromStringList(studentIds)}
//                  }}
//     };

//     sendRequest(request);  // 发送请求
// }

// // 更新学生信息
// void ClientWindow::onUpdateStudent()
// {
//     // 检查选择
//     QModelIndexList selected = ui->studentsTableView->selectionModel()->selectedRows();
//     if (selected.isEmpty()) {
//         QMessageBox::warning(this, "提示", "请先选择要编辑的学生");
//         return;
//     }

//     int row = selected.first().row();  // 只编辑第一个选中的学生
//     QAbstractItemModel *model = ui->studentsTableView->model();

//     // 创建编辑对话框并填充当前数据
//     StudentDialog dialog(this, true); // true表示编辑模式
//     dialog.setStudentData(
//         model->index(row, 0).data().toString(),  // 学号
//         model->index(row, 1).data().toString(),  // 姓名
//         model->index(row, 2).data().toString(),  // 性别
//         model->index(row, 3).data().toInt(),     // 年龄
//         model->index(row, 4).data().toString(),  // 院系
//         model->index(row, 5).data().toString(),  // 专业
//         model->index(row, 6).data().toString(),  // 班级
//         model->index(row, 7).data().toString(),  // 电话
//         model->index(row, 8).data().toString()   // 邮箱
//         );

//     if (dialog.exec() == QDialog::Accepted) {
//         // 构造更新请求
//         QJsonObject request {
//             {"type", "update_student"},
//             {"data", QJsonObject {
//                          {"student_id", model->index(row, 0).data().toString()}, // 使用原学号
//                          {"name", dialog.name()},
//                          {"gender", dialog.gender()},
//                          {"age", dialog.age()},
//                          {"department", dialog.department()},
//                          {"major", dialog.major()},
//                          {"class", dialog.className()},
//                          {"phone", dialog.phone()},
//                          {"email", dialog.email()}
//                      }}
//         };

//         sendRequest(request);  // 发送请求
//     }
// }

// // 查询学生
// void ClientWindow::onQueryStudents()
// {
//     QString keyword = ui->searchEdit->text();

//     // 构造查询请求
//     QJsonObject request {
//         {"type", "query_students"},
//         {"data", QJsonObject {
//                      {"keyword", keyword},
//                      {"page", 1},
//                      {"pageSize", 100},
//                      {"role", m_role},
//                      {"username", m_username}
//                  }}
//     };

//     sendRequest(request);  // 发送请求
// }

// // 导出数据（未实现）
// void ClientWindow::onExportData()
// {
//     QString fileName = QFileDialog::getSaveFileName(this, "导出文件", "", "Excel文件 (*.xlsx)");
//     if (fileName.isEmpty()) return;

//     // 获取所有学生ID
//     QJsonArray studentIds;
//     QAbstractItemModel *model = ui->studentsTableView->model();
//     for (int row = 0; row < model->rowCount(); ++row) {
//         studentIds.append(model->index(row, 0).data().toString());
//     }

//     QJsonObject request {
//         {"type", "export_data"},
//         {"data", QJsonObject {
//                      {"format", "excel"},
//                      {"student_ids", studentIds}
//                  }}
//     };

//     // ui->exportProgressBar->setValue(0);
//     // ui->exportStatusLabel->setText("准备导出...");

//     sendRequest(request);
// }

// // 切换主题
// void ClientWindow::onThemeChanged(int index)
// {
//     // 获取主题名称并显示消息
//     QString theme = ui->themeComboBox->itemText(index);
//     showMessage(QString("已切换至%1主题").arg(theme));

//     // 应用对应主题
//     switch (index) {
//     case Light:
//         applyLightTheme();
//         break;
//     case Dark:
//         applyDarkTheme();
//         break;
//     case Blue:
//         applyBlueTheme();
//         break;
//     default:
//         applyLightTheme();
//     }
// }

// // 应用浅色主题
// void ClientWindow::applyLightTheme()
// {
//     qApp->setStyle(QStyleFactory::create("Fusion"));

//     // 创建浅色调色板
//     QPalette palette;
//     palette.setColor(QPalette::Window, QColor(240, 240, 240));
//     palette.setColor(QPalette::WindowText, Qt::black);
//     palette.setColor(QPalette::Base, Qt::white);
//     palette.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
//     palette.setColor(QPalette::ToolTipBase, Qt::white);
//     palette.setColor(QPalette::ToolTipText, Qt::black);
//     palette.setColor(QPalette::Text, Qt::black);
//     palette.setColor(QPalette::Button, QColor(240, 240, 240));
//     palette.setColor(QPalette::ButtonText, Qt::black);
//     palette.setColor(QPalette::BrightText, Qt::red);
//     palette.setColor(QPalette::Link, QColor(42, 130, 218));
//     palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//     palette.setColor(QPalette::HighlightedText, Qt::white);

//     qApp->setPalette(palette);
//     qApp->setStyleSheet(""); // 清除样式表
// }

// // 应用深色主题
// void ClientWindow::applyDarkTheme()
// {
//     qApp->setStyle(QStyleFactory::create("Fusion"));

//     // 创建深色调色板
//     QPalette palette;
//     palette.setColor(QPalette::Window, QColor(53, 53, 53));
//     palette.setColor(QPalette::WindowText, Qt::white);
//     palette.setColor(QPalette::Base, QColor(25, 25, 25));
//     palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
//     palette.setColor(QPalette::ToolTipBase, Qt::black);
//     palette.setColor(QPalette::ToolTipText, Qt::white);
//     palette.setColor(QPalette::Text, Qt::white);
//     palette.setColor(QPalette::Button, QColor(53, 53, 53));
//     palette.setColor(QPalette::ButtonText, Qt::white);
//     palette.setColor(QPalette::BrightText, Qt::red);
//     palette.setColor(QPalette::Link, QColor(42, 130, 218));
//     palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//     palette.setColor(QPalette::HighlightedText, Qt::black);

//     qApp->setPalette(palette);

//     // 额外的暗色样式表
//     QString darkStyle = R"(
//         QToolTip { color: #ffffff; background-color: #2a2a2a; border: 1px solid #767676; }
//         QMenu { background-color: #353535; color: white; }
//         QMenu::item:selected { background-color: #2a82da; }
//         QTableView { alternate-background-color: #2d2d2d; }
//         QHeaderView::section { background-color: #454545; color: white; }
//     )";
//     qApp->setStyleSheet(darkStyle);
// }

// // 应用蓝色主题
// void ClientWindow::applyBlueTheme()
// {
//     qApp->setStyle(QStyleFactory::create("Fusion"));

//     // 创建蓝色调色板
//     QPalette palette;
//     palette.setColor(QPalette::Window, QColor(240, 245, 255));
//     palette.setColor(QPalette::WindowText, QColor(20, 20, 100));
//     palette.setColor(QPalette::Base, Qt::white);
//     palette.setColor(QPalette::AlternateBase, QColor(230, 235, 255));
//     palette.setColor(QPalette::ToolTipBase, Qt::white);
//     palette.setColor(QPalette::ToolTipText, QColor(20, 20, 100));
//     palette.setColor(QPalette::Text, QColor(20, 20, 100));
//     palette.setColor(QPalette::Button, QColor(200, 210, 255));
//     palette.setColor(QPalette::ButtonText, QColor(20, 20, 100));
//     palette.setColor(QPalette::BrightText, Qt::red);
//     palette.setColor(QPalette::Link, QColor(0, 100, 200));
//     palette.setColor(QPalette::Highlight, QColor(0, 100, 200));
//     palette.setColor(QPalette::HighlightedText, Qt::white);

//     qApp->setPalette(palette);

//     // 额外的蓝色样式表
//     QString blueStyle = R"(
//         QToolTip { color: #141464; background-color: #e6f0ff; border: 1px solid #6496c8; }
//         QMenu { background-color: #f0f5ff; color: #141464; }
//         QMenu::item:selected { background-color: #6496c8; color: white; }
//         QTableView { alternate-background-color: #e6ebff; }
//         QHeaderView::section { background-color: #c8d2ff; color: #141464; }
//     )";
//     qApp->setStyleSheet(blueStyle);
// }

// // 发送请求到服务器
// void ClientWindow::sendRequest(const QJsonObject &request)
// {
//     // 检查连接状态
//     if (m_socket->state() != QAbstractSocket::ConnectedState) {
//         if (!m_connecting) {
//             // 触发重新连接
//             m_pendingRequest = request;
//             onLoginButtonClicked();
//         } else {
//             m_pendingRequest = request; // 更新暂存请求
//         }
//         return;
//     }

//     // 发送JSON请求
//     QJsonDocument doc(request);
//     if (m_socket->write(doc.toJson()) == -1) {
//         showError("发送请求失败: " + m_socket->errorString());
//     } else {
//         m_socket->waitForBytesWritten(1000); // 等待数据写入
//     }
// }

// // 显示错误消息
// void ClientWindow::showError(const QString &message)
// {
//     QMessageBox::critical(this, "错误", message);
//     ui->statusLabel->setText("错误: " + message);
// }

// // 显示普通消息
// void ClientWindow::showMessage(const QString &message)
// {
//     ui->statusLabel->setText(message);
// }

// // 更新界面权限
// void ClientWindow::updatePermissions()
// {
//     bool isAdmin = (m_role == "admin");

//     // 管理员才显示的操作按钮
//     ui->addButton->setEnabled(isAdmin);
//     ui->deleteButton->setEnabled(isAdmin);
//     ui->updateButton->setEnabled(isAdmin);
//     ui->addToExportBtn->setEnabled(isAdmin);

//     // 学生角色提示
//     if (!isAdmin) {
//         ui->searchEdit->setPlaceholderText("只能查询自己的信息");
//     }
// }

// // 初始化/更新图表
// void ClientWindow::setupCharts()
// {
//     // 年龄分布图表
//     QAbstractItemModel *model = ui->studentsTableView->model();
//     QMap<int, int> ageCount;  // 年龄-人数映射

//     // 统计年龄分布
//     for (int row = 0; row < model->rowCount(); ++row) {
//         int age = model->index(row, 3).data().toInt();
//         ageCount[age]++;
//     }

//     // 创建柱状图系列
//     QBarSeries *ageSeries = new QBarSeries();
//     QBarSet *ageSet = new QBarSet("人数");
//     QStringList ageCategories;

//     // 填充数据
//     for (auto it = ageCount.begin(); it != ageCount.end(); ++it) {
//         *ageSet << it.value();
//         ageCategories << QString("%1岁").arg(it.key());
//     }

//     ageSeries->append(ageSet);

//     // 创建年龄图表
//     QChart *ageChart = new QChart();
//     ageChart->addSeries(ageSeries);
//     ageChart->setTitle("年龄分布");

//     // 设置X轴（分类轴）
//     QBarCategoryAxis *ageAxisX = new QBarCategoryAxis();
//     ageAxisX->append(ageCategories);
//     ageChart->addAxis(ageAxisX, Qt::AlignBottom);
//     ageSeries->attachAxis(ageAxisX);

//     // 设置Y轴（数值轴）
//     QValueAxis *ageAxisY = new QValueAxis();
//     ageChart->addAxis(ageAxisY, Qt::AlignLeft);
//     ageSeries->attachAxis(ageAxisY);

//     m_ageChartView->setChart(ageChart);

//     // 性别比例饼图
//     QPieSeries *genderSeries = new QPieSeries();

//     // 统计性别分布
//     int maleCount = 0, femaleCount = 0;
//     for (int row = 0; row < model->rowCount(); ++row) {
//         QString gender = model->index(row, 2).data().toString().toLower();
//         if (gender == "male" || gender == "男") maleCount++;
//         else if (gender == "female" || gender == "女") femaleCount++;
//     }

//     int total = maleCount + femaleCount;

//     // 处理无数据情况
//     if (total == 0) {
//         QPieSlice *noDataSlice = genderSeries->append("无数据", 1);
//         noDataSlice->setLabelVisible(true);
//         noDataSlice->setLabel("暂无学生数据");
//         noDataSlice->setColor(Qt::lightGray);
//     } else {
//         // 添加数据切片
//         QPieSlice *maleSlice = genderSeries->append("男生", maleCount);
//         QPieSlice *femaleSlice = genderSeries->append("女生", femaleCount);

//         // 设置标签格式
//         QString labelFormat = "%1\n%2人\n%3%";
//         maleSlice->setLabel(labelFormat.arg("男生").arg(maleCount).arg(100.0*maleCount/(maleCount + femaleCount)));
//         femaleSlice->setLabel(labelFormat.arg("女生").arg(femaleCount).arg(100.0*femaleCount/(maleCount + femaleCount)));

//         // 标签样式
//         maleSlice->setLabelVisible(true);
//         femaleSlice->setLabelVisible(true);
//         maleSlice->setLabelPosition(QPieSlice::LabelOutside);
//         femaleSlice->setLabelPosition(QPieSlice::LabelOutside);

//         // 设置颜色
//         maleSlice->setColor(QColor(64, 158, 255)); // 蓝色
//         femaleSlice->setColor(QColor(255, 128, 192)); // 粉色

//         // 悬停效果
//         maleSlice->setBorderWidth(2);
//         femaleSlice->setBorderWidth(2);
//         connect(genderSeries, &QPieSeries::hovered, [](QPieSlice *slice, bool hovered) {
//             slice->setExploded(hovered);
//             slice->setBorderWidth(hovered ? 3 : 2);
//         });
//     }

//     // 创建性别图表
//     QChart *genderChart = new QChart();
//     genderChart->addSeries(genderSeries);
//     genderChart->setTitle("性别比例");
//     genderChart->legend()->setVisible(true);
//     genderChart->legend()->setAlignment(Qt::AlignBottom);
//     genderChart->setAnimationOptions(QChart::SeriesAnimations);

//     m_genderChartView->setChart(genderChart);
//     m_genderChartView->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
// }


// // 添加到导出列表
// void ClientWindow::onAddToExportList() {
//     // 获取表格中的学生数据
//     QJsonArray studentsArray = getStudentsDataFromTable();
//     if (studentsArray.isEmpty()) {
//         QMessageBox::warning(this, "提示", "没有可导出的学生数据");
//         return;
//     }

//     // 生成基础文件名（带时间戳）
//     QString baseName = QString("students_%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));
//     QString format = ui->formatComboBox->currentText();

//     // 创建导出任务
//     ExportTask* task = new ExportTask(baseName, format, m_exportPath, this);
//     task->setStudentsData(studentsArray);
//     m_exportModel->addTask(task);  // 添加到模型

//     showMessage(QString("已添加导出任务，共%1条记录").arg(studentsArray.size()));
// }

// // 从表格获取学生数据（JSON数组）
// QJsonArray ClientWindow::getStudentsDataFromTable() const {
//     QJsonArray studentsArray;
//     QAbstractItemModel* model = ui->studentsTableView->model();

//     // 遍历所有行
//     for (int row = 0; row < model->rowCount(); ++row) {
//         QJsonObject student;
//         // 填充学生属性
//         student["student_id"] = model->index(row, 0).data().toString();
//         student["name"] = model->index(row, 1).data().toString();
//         student["gender"] = model->index(row, 2).data().toString();
//         student["age"] = model->index(row, 3).data().toInt();
//         student["department"] = model->index(row, 4).data().toString();
//         student["major"] = model->index(row, 5).data().toString();
//         student["class"] = model->index(row, 6).data().toString();
//         student["phone"] = model->index(row, 7).data().toString();
//         student["email"] = model->index(row, 8).data().toString();

//         studentsArray.append(student);  // 添加到数组
//     }

//     return studentsArray;
// }

// // 处理导出路径变化
// void ClientWindow::onExportPathChanged() {
//     m_exportPath = ui->exportPathEdit->text();  // 更新导出路径
// }

// // 浏览导出路径
// void ClientWindow::onBrowseExportPath() {
//     // 打开目录选择对话框
//     QString dir = QFileDialog::getExistingDirectory(this, "选择导出目录", m_exportPath);
//     if (!dir.isEmpty()) {
//         ui->exportPathEdit->setText(dir);  // 更新路径显示
//     }
// }

// // 移除导出任务
// void ClientWindow::onRemoveExportTask(const QModelIndex& index) {
//     if (index.column() == 4) { // 删除按钮列
//         m_exportModel->removeRow(index.row());  // 从模型移除
//     }
// }

// // 执行全部导出
// void ClientWindow::onExportAll() {
//     // 获取待处理任务
//     QList<ExportTask*> tasks = m_exportModel->pendingTasks();
//     if (tasks.isEmpty()) {
//         QMessageBox::information(this, "提示", "没有待导出的任务");
//         return;
//     }

//     // 为每个任务创建工作线程
//     foreach (ExportTask* task, tasks) {
//         ExportWorker* worker = new ExportWorker(task);

//         // 连接完成信号
//         connect(worker, &ExportWorker::finished, this, [this, worker]() {
//             worker->deleteLater();  // 清理线程
//             onExportFinished();     // 处理完成
//         });

//         // 连接错误信号
//         connect(worker, &ExportWorker::error, this, [this, worker](const QString& err) {
//             showError(err);         // 显示错误
//             worker->deleteLater();  // 清理线程
//         });

//         // 使用线程池执行任务
//         m_exportPool.start(worker);
//     }
// }

// // 处理导出完成
// void ClientWindow::onExportFinished() {
//     showMessage(QString("导出任务已完成, 一共%1条任务.").arg(m_exportModel->pendingTasks().size()));
// }
