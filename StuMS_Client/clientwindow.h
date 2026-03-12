// #ifndef CLIENTWINDOW_H
// #define CLIENTWINDOW_H

// #include <QMainWindow>
// #include <QTcpSocket>      // TCP套接字支持
// #include <QJsonDocument>    // JSON数据处理
// #include <QJsonObject>      // JSON对象操作
// #include <QChartView>       // 图表显示组件
// #include <QThread>          // 线程支持
// #include <QThreadPool>      // 线程池管理

// QT_BEGIN_NAMESPACE
// namespace Ui { class ClientWindow; }  // 前置声明UI命名空间
// QT_END_NAMESPACE

// class ExportTaskModel;  // 前置声明导出任务模型类

// // 主客户端窗口类，继承自QMainWindow
// class ClientWindow : public QMainWindow
// {
//     Q_OBJECT  // 启用Qt元对象系统

// public:
//     // 主题枚举定义
//     enum Theme { Light, Dark, Blue, Custom };
//     Q_ENUM(Theme)  // 注册枚举到元对象系统

// public:
//     ClientWindow(QWidget *parent = nullptr);  // 构造函数
//     ~ClientWindow();  // 析构函数

//     // 从表格中获取学生数据的JSON数组
//     QJsonArray getStudentsDataFromTable() const;

// signals:
//     // 连接建立成功信号
//     void connectionEstablished();

// private slots:
//     // 连接建立处理槽
//     void handleConnectionEstablished();
//     // 连接错误处理槽
//     void handleConnectionError(QAbstractSocket::SocketError error);

//     // UI按钮点击槽
//     void onLoginButtonClicked();      // 登录按钮
//     void onConnected();               // TCP连接成功
//     void onDisconnected();            // TCP断开连接
//     void onReadyRead();               // 读取服务器数据
//     void onAddStudent();              // 添加学生
//     void onDeleteStudent();           // 删除学生
//     void onUpdateStudent();           // 更新学生信息
//     void onQueryStudents();           // 查询学生
//     void onExportData();              // 导出数据
//     void onThemeChanged(int index);   // 主题切换
//     void onAddToExportList();         // 添加导出任务
//     void onExportPathChanged();       // 导出路径变更
//     void onBrowseExportPath();        // 浏览导出路径
//     void onExportAll();               // 执行全部导出
//     void onRemoveExportTask(const QModelIndex& index);  // 移除导出任务
//     void onExportFinished();          // 导出完成

//     // 服务器响应处理槽
//     void handleLoginResponse(const QJsonObject &response);       // 登录响应
//     void handleStudentsData(const QJsonObject &response);        // 学生数据响应
//     void handleExportProgress(const QJsonObject &response);      // 导出进度响应

// private:
//     Ui::ClientWindow *ui;          // UI指针
//     QTcpSocket *m_socket;          // TCP套接字对象
//     QString m_username;            // 当前用户名
//     QString m_role;                // 用户角色
//     bool m_authenticated;          // 认证状态标志

//     bool m_connecting = false;     // 连接中标志
//     QJsonObject m_pendingRequest;  // 暂存待发送的请求
//     int m_reconnectAttempts = 0;   // 重连尝试次数

//     ExportTaskModel* m_exportModel;  // 导出任务模型指针
//     QThreadPool m_exportPool;        // 导出线程池
//     QString m_exportPath;            // 导出路径

//     // 初始化方法
//     void setupUI();         // 初始化UI组件
//     void setupCharts();     // 初始化图表
//     void updatePermissions();  // 更新界面权限

//     // 网络通信方法
//     void sendRequest(const QJsonObject &request);  // 发送请求到服务器

//     // UI工具方法
//     void showError(const QString &message);    // 显示错误提示
//     void showMessage(const QString &message);  // 显示信息提示

//     // 图表组件
//     QChartView *m_ageChartView;     // 年龄分布图表视图
//     QChartView *m_genderChartView;  // 性别分布图表视图

//     // 主题应用方法
//     void applyLightTheme();  // 应用浅色主题
//     void applyDarkTheme();   // 应用深色主题
//     void applyBlueTheme();   // 应用蓝色主题
// };

// #endif // CLIENTWINDOW_H
