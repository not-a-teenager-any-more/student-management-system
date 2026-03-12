#include "infowidget.h"
#include <QHeaderView>
#include <QFormLayout>
#include <QDebug>
#include <QSqlError>
#include <QSizePolicy>
#include <QShortcut>
#include <QGraphicsDropShadowEffect>
#include <QButtonGroup>

InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent)
{
    if (!initDatabase()) {
        QMessageBox::critical(this, "数据库错误", "无法连接到通知数据库！");
    }
    setupUI();
    refreshAll();
}

InfoWidget::~InfoWidget()
{
    db.close();
}

bool InfoWidget::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("notifications.db");
    if (!db.open()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT,
            content TEXT,
            time DATETIME,
            read INTEGER DEFAULT 0,
            source TEXT,
            isTop INTEGER DEFAULT 0,
            sender TEXT,
            deadline TEXT,
            type TEXT
        )
    )");

    // --- 模拟数据插入逻辑 ---
    QSqlQuery checkQuery("SELECT COUNT(*) FROM notifications");
    if (checkQuery.next() && checkQuery.value(0).toInt() == 0) {
        qDebug() << "初始化默认模拟数据...";
        QSqlQuery insert;
        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString tomorrow = QDateTime::currentDateTime().addDays(1).toString("yyyy-MM-dd HH:mm");
        QString nextWeek = QDateTime::currentDateTime().addDays(7).toString("yyyy-MM-dd HH:mm");

        // 系统通知
        insert.prepare("INSERT INTO notifications (title, content, time, source, type, read) VALUES (?, ?, ?, ?, ?, ?)");
        insert.addBindValue("系统维护提醒");
        insert.addBindValue("系统将于今晚24:00进行服务器升级，预计耗时2小时，请提前保存工作。");
        insert.addBindValue(now);
        insert.addBindValue("学生管理");
        insert.addBindValue("notification");
        insert.addBindValue(0);
        insert.exec();

        insert.addBindValue("本月数据报表已生成");
        insert.addBindValue("2026年1月的数据分析报表已生成，请前往数据分析模块查看。");
        insert.addBindValue(now);
        insert.addBindValue("数据分析");
        insert.addBindValue("notification");
        insert.addBindValue(0);
        insert.exec();

        insert.addBindValue("Excel导出成功");
        insert.addBindValue("您请求的学生名单导出任务已完成。");
        insert.addBindValue(now);
        insert.addBindValue("数据导出");
        insert.addBindValue("notification");
        insert.addBindValue(1);
        insert.exec();

        // 待办事项
        insert.prepare("INSERT INTO notifications (title, content, deadline, type, read) VALUES (?, ?, ?, ?, ?)");
        insert.addBindValue("完成C++课程大作业");
        insert.addBindValue("需要完成Qt界面的编写和数据库连接功能。");
        insert.addBindValue(tomorrow);
        insert.addBindValue("todo");
        insert.addBindValue(0);
        insert.exec();

        insert.addBindValue("归还图书馆书籍");
        insert.addBindValue("借阅的《设计模式》需要归还。");
        insert.addBindValue(nextWeek);
        insert.addBindValue("todo");
        insert.addBindValue(0);
        insert.exec();

        insert.addBindValue("提交选课申请");
        insert.addBindValue("需要在教务系统确认下学期的选课。");
        insert.addBindValue(QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd HH:mm"));
        insert.addBindValue("todo");
        insert.addBindValue(0);
        insert.exec();

        // 公告
        insert.prepare("INSERT INTO notifications (title, content, time, isTop, type) VALUES (?, ?, ?, ?, ?)");
        insert.addBindValue("2026年寒假放假通知");
        insert.addBindValue("根据校历安排，寒假将于1月15日开始，2月20日开学。请各位同学离校前关好门窗。");
        insert.addBindValue(now);
        insert.addBindValue(1);
        insert.addBindValue("announcement");
        insert.exec();

        insert.addBindValue("关于举办校园歌手大赛的通知");
        insert.addBindValue("第十届校园歌手大赛将于本周五晚在活动中心举行，欢迎大家踊跃报名。");
        insert.addBindValue(now);
        insert.addBindValue(0);
        insert.addBindValue("announcement");
        insert.exec();
    }
    return true;
}

void InfoWidget::setupUI()
{
    // --- 1. 高级样式表定义 ---
    setStyleSheet(R"(
        QWidget {
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
            font-size: 14px;
            color: #333333; /* 全局默认字体颜色设为深灰，防止白字 */
        }
        /* 整体背景 */
        InfoWidget {
            background-color: #f5f7fa;
        }

        /* 顶部大标题 */
        QLabel#PageTitle {
            font-size: 24px;
            font-weight: bold;
            color: #2c3e50;
            padding: 10px 0;
        }

        /* 未读计数标签 */
        QLabel#UnreadCount {
            color: #e74c3c;
            font-weight: bold;
            background-color: #fadbd8;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 12px;
        }

        /* 顶部功能按钮 (刷新/清空) - 扁平化风格 */
        QPushButton#ToolBtn {
            background-color: white;
            color: #555;
            border: 1px solid #dcdfe6;
            border-radius: 4px;
            padding: 6px 12px;
            font-weight: normal;
        }
        QPushButton#ToolBtn:hover {
            color: #3498db;
            border-color: #3498db;
            background-color: #ecf5ff;
        }
        QPushButton#ToolBtn:pressed {
            background-color: #d9ecff;
        }

        /* 选项卡切换按钮 - 底部线条风格 */
        QPushButton#TabBtn {
            background-color: transparent;
            color: #606266;
            border: none;
            padding: 10px 20px;
            font-size: 15px;
            font-weight: bold;
            border-bottom: 3px solid transparent;
        }
        QPushButton#TabBtn:hover {
            color: #3498db;
        }
        QPushButton#TabBtn:checked {
            color: #3498db;
            border-bottom: 3px solid #3498db;
        }

        /* 内容卡片区域 */
        QFrame#ContentCard {
            background-color: white;
            border-radius: 8px;
            border: 1px solid #e4e7ed;
        }

        /* 列表和表格通用样式 - 去除边框，增加留白 */
        QListWidget, QTableWidget {
            border: none;
            outline: none;
            background-color: white;
            color: #333333; /* 强制列表文字颜色 */
        }

        /* 列表项样式 */
        QListWidget::item {
            padding: 12px;
            border-bottom: 1px solid #f2f6fc;
            margin: 0px 5px;
            border-radius: 4px;
            color: #333333; /* 强制 Item 文字颜色 */
        }
        QListWidget::item:hover {
            background-color: #f5f7fa;
        }
        QListWidget::item:selected {
            background-color: #ecf5ff;
            color: #3498db;
        }

        /* 表头样式 */
        QHeaderView::section {
            background-color: #5c6bc0;
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
            font-size: 14px;
        }
        /* 表格行样式 */
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #f0f0f0;
            color: #333333; /* 强制表格文字颜色 */
        }
        QTableWidget::item:selected {
            background-color: #e8f0fe;
            color: #333;
        }

        /* 底部/主要操作按钮 (蓝色实心) */
        QPushButton#ActionBtn {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 14px;
        }
        QPushButton#ActionBtn:hover {
            background-color: #2980b9;
        }

        /* 下拉框美化 */
        QComboBox {
            border: 1px solid #dcdfe6;
            border-radius: 4px;
            padding: 5px;
            min-width: 100px;
            color: #333;
            background-color: white;
        }
        QComboBox::drop-down {
            border: none;
        }

        /* 删除按钮 - 红色文字 */
        QPushButton#DeleteBtn {
            background-color: transparent;
            color: #f56c6c;
            border: none;
            font-weight: bold;
        }
        QPushButton#DeleteBtn:hover {
            text-decoration: underline;
        }
    )");

    // --- 2. 布局构建 ---

    // 2.1 顶部标题栏 (Header)
    QHBoxLayout *headerLayout = new QHBoxLayout;

    QLabel *lblTitle = new QLabel("系统通知");
    lblTitle->setObjectName("PageTitle");

    lblUnread = new QLabel("未读: 0");
    lblUnread->setObjectName("UnreadCount");

    QPushButton *btnRefresh = new QPushButton("刷新");
    QPushButton *btnClear = new QPushButton("清空已读");
    QPushButton *btnMarkAll = new QPushButton("全部已读");

    // 应用 ToolBtn 样式
    btnRefresh->setObjectName("ToolBtn");
    btnClear->setObjectName("ToolBtn");
    btnMarkAll->setObjectName("ToolBtn");

    headerLayout->addWidget(lblTitle);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(lblUnread);
    headerLayout->addStretch();
    headerLayout->addWidget(btnRefresh);
    headerLayout->addWidget(btnClear);
    headerLayout->addWidget(btnMarkAll);

    // 2.2 导航标签栏 (Tabs)
    QHBoxLayout *tabLayout = new QHBoxLayout;
    tabLayout->setSpacing(20);
    tabLayout->setContentsMargins(10, 0, 0, 10); // 左侧留点空隙

    QPushButton *btnNotifications = new QPushButton("通知消息");
    QPushButton *btnTodos = new QPushButton("待办事项");
    QPushButton *btnAnnouncements = new QPushButton("公告栏");

    btnNotifications->setObjectName("TabBtn");
    btnTodos->setObjectName("TabBtn");
    btnAnnouncements->setObjectName("TabBtn");

    btnNotifications->setCheckable(true);
    btnTodos->setCheckable(true);
    btnAnnouncements->setCheckable(true);
    btnNotifications->setChecked(true); // 默认选中第一个

    // 互斥逻辑
    QButtonGroup *tabGroup = new QButtonGroup(this);
    tabGroup->addButton(btnNotifications);
    tabGroup->addButton(btnTodos);
    tabGroup->addButton(btnAnnouncements);
    tabGroup->setExclusive(true);

    tabLayout->addWidget(btnNotifications);
    tabLayout->addWidget(btnTodos);
    tabLayout->addWidget(btnAnnouncements);
    tabLayout->addStretch();

    // 2.3 主要内容区 (Card)
    QFrame *contentCard = new QFrame;
    contentCard->setObjectName("ContentCard");

    // 给卡片添加轻微阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 20)); // 淡淡的阴影
    shadow->setOffset(0, 2);
    contentCard->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(contentCard);
    cardLayout->setContentsMargins(0, 10, 0, 10); // 卡片内部边距

    stackedWidget = new QStackedWidget;

    // --- 页面 1: 通知 ---
    QWidget *pageNotifications = new QWidget;
    QVBoxLayout *vNotifications = new QVBoxLayout(pageNotifications);

    // 筛选栏
    QHBoxLayout *filterLayout = new QHBoxLayout;
    QLabel *lblFilter = new QLabel("来源筛选:");
    cmbSource = new QComboBox;
    cmbSource->addItems({"全部模块","学生管理","数据分析","数据导出"});
    filterLayout->addWidget(lblFilter);
    filterLayout->addWidget(cmbSource);
    filterLayout->addStretch();

    // 修改：显式显示快捷键
    QPushButton *btnAddNotify = new QPushButton("+ 添加通知 (Ctrl+N)");
    btnAddNotify->setObjectName("ActionBtn");
    filterLayout->addWidget(btnAddNotify);

    listNotifications = new QListWidget;
    listNotifications->setFrameShape(QFrame::NoFrame); // 去除列表自带边框

    vNotifications->addLayout(filterLayout);
    vNotifications->addWidget(listNotifications);
    vNotifications->setContentsMargins(20, 10, 20, 10);

    // --- 页面 2: 待办 (表格优化) ---
    QWidget *pageTodos = new QWidget;
    QVBoxLayout *vTodos = new QVBoxLayout(pageTodos);

    QHBoxLayout *todoToolLayout = new QHBoxLayout;
    todoToolLayout->addStretch();

    // 修改：显式显示快捷键
    QPushButton *btnAddTodo = new QPushButton("+ 添加待办 (Ctrl+T)");
    btnAddTodo->setObjectName("ActionBtn");
    todoToolLayout->addWidget(btnAddTodo);

    tableTodos = new QTableWidget;
    tableTodos->setColumnCount(4);
    tableTodos->setHorizontalHeaderLabels({"标题", "截止日期", "状态", "操作"});
    tableTodos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableTodos->setAlternatingRowColors(true); // 开启交替行背景
    tableTodos->setShowGrid(false); // 去除网格线
    tableTodos->setFrameShape(QFrame::NoFrame);

    // 表格交互优化
    tableTodos->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTodos->setSelectionMode(QAbstractItemView::SingleSelection);
    tableTodos->setFocusPolicy(Qt::NoFocus);
    tableTodos->verticalHeader()->setVisible(false);
    tableTodos->verticalHeader()->setDefaultSectionSize(50); // 增加行高

    QHeaderView *header = tableTodos->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch); // 标题自适应
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    tableTodos->setColumnWidth(1, 160);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    tableTodos->setColumnWidth(2, 100);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    tableTodos->setColumnWidth(3, 100);

    vTodos->addLayout(todoToolLayout);
    vTodos->addWidget(tableTodos);
    vTodos->setContentsMargins(20, 10, 20, 10);

    // --- 页面 3: 公告 ---
    QWidget *pageAnnouncements = new QWidget;
    QVBoxLayout *vAnnouncements = new QVBoxLayout(pageAnnouncements);

    QHBoxLayout *announceToolLayout = new QHBoxLayout;
    announceToolLayout->addStretch();

    // 修改：显式显示快捷键
    QPushButton *btnAddAnnounce = new QPushButton("+ 发布公告 (Ctrl+L)");
    btnAddAnnounce->setObjectName("ActionBtn");
    announceToolLayout->addWidget(btnAddAnnounce);

    listAnnouncements = new QListWidget;
    listAnnouncements->setFrameShape(QFrame::NoFrame);

    vAnnouncements->addLayout(announceToolLayout);
    vAnnouncements->addWidget(listAnnouncements);
    vAnnouncements->setContentsMargins(20, 10, 20, 10);

    // 添加到 Stack
    stackedWidget->addWidget(pageNotifications);
    stackedWidget->addWidget(pageTodos);
    stackedWidget->addWidget(pageAnnouncements);

    cardLayout->addWidget(stackedWidget);

    // 2.4 主布局组合
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30); // 整体外边距
    mainLayout->setSpacing(10);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(tabLayout);
    mainLayout->addWidget(contentCard);

    // --- 3. 信号槽连接 ---
    connect(btnRefresh, &QPushButton::clicked, this, &InfoWidget::refreshAll);
    connect(btnClear, &QPushButton::clicked, this, &InfoWidget::clearRead);
    connect(btnMarkAll, &QPushButton::clicked, this, &InfoWidget::markAllRead);
    connect(cmbSource, &QComboBox::currentIndexChanged, this, &InfoWidget::filterSource);
    connect(listNotifications, &QListWidget::itemClicked, this, &InfoWidget::onNotificationClicked);
    connect(tableTodos, &QTableWidget::cellClicked, this, &InfoWidget::onTodoClicked);
    connect(listAnnouncements, &QListWidget::itemClicked, this, &InfoWidget::onAnnouncementClicked);

    // Tab 切换
    connect(btnNotifications, &QPushButton::clicked, this, &InfoWidget::switchToNotifications);
    connect(btnTodos, &QPushButton::clicked, this, &InfoWidget::switchToTodos);
    connect(btnAnnouncements, &QPushButton::clicked, this, &InfoWidget::switchToAnnouncements);

    connect(btnAddNotify, &QPushButton::clicked, this, &InfoWidget::addNotification);
    connect(btnAddTodo, &QPushButton::clicked, this, &InfoWidget::addTodo);
    connect(btnAddAnnounce, &QPushButton::clicked, this, &InfoWidget::addAnnouncement);

    listAnnouncements->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listAnnouncements, &QListWidget::customContextMenuRequested,
            this, &InfoWidget::showAnnouncementContextMenu);

    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(addNotification()));
    new QShortcut(QKeySequence("Ctrl+T"), this, SLOT(addTodo()));
    new QShortcut(QKeySequence("Ctrl+L"), this, SLOT(addAnnouncement()));
}

void InfoWidget::showAnnouncementContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = listAnnouncements->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    menu.setStyleSheet("QMenu { background: white; border: 1px solid #ddd; } QMenu::item { padding: 5px 20px; color: black; } QMenu::item:selected { background: #e8f0fe; }");
    QAction *deleteAction = menu.addAction("删除公告");
    connect(deleteAction, &QAction::triggered, this, [this, item]() {
        int id = item->data(Qt::UserRole).toInt();
        deleteAnnouncement(id);
    });
    menu.exec(listAnnouncements->mapToGlobal(pos));
}

void InfoWidget::switchToNotifications()
{
    stackedWidget->setCurrentIndex(0);
    filterSource(cmbSource->currentIndex());
}

void InfoWidget::switchToTodos()
{
    stackedWidget->setCurrentIndex(1);
    loadTodos();
}

void InfoWidget::switchToAnnouncements()
{
    stackedWidget->setCurrentIndex(2);
    loadAnnouncements();
}

void InfoWidget::refreshAll()
{
    loadNotifications();
    loadAnnouncements();
    loadTodos();
    updateUnreadCount();
    if (stackedWidget->currentIndex() == 0) {
        filterSource(cmbSource->currentIndex());
    }
}

void InfoWidget::updateUnreadCount()
{
    int count = 0;
    for (const auto &n : notifications) if (!n.read) count++;
    lblUnread->setText(QString("未读: %1").arg(count));
    // 如果没有未读，标签变灰；有未读，变红
    if (count == 0) {
        lblUnread->setStyleSheet("QLabel#UnreadCount { background-color: #e0e0e0; color: #666; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");
    } else {
        lblUnread->setStyleSheet("QLabel#UnreadCount { background-color: #fadbd8; color: #e74c3c; border-radius: 4px; padding: 4px 8px; font-weight: bold; }");
    }
}

void InfoWidget::filterSource(int index)
{
    Q_UNUSED(index);
    QString source = cmbSource->currentText();
    listNotifications->clear();
    for (const auto &n : notifications) {
        if (source == "全部模块" || n.source == source) {

            // 简单的文本展示
            QString display = QString("%1 [%2] %3  ---  %4").arg(n.read ? "✔" : "●").arg(n.source).arg(n.title).arg(n.time.toString("MM-dd HH:mm"));
            QListWidgetItem *item = new QListWidgetItem(display);

            item->setData(Qt::UserRole, n.id);
            item->setData(Qt::UserRole + 1, n.content);

            if (!n.read) {
                // 未读：深色
                item->setForeground(QColor("#2c3e50"));
                QFont f = item->font(); f.setBold(true); item->setFont(f);
            } else {
                // 已读：中灰色 (之前是 #909399 太淡了，改为 #555555)
                item->setForeground(QColor("#555555"));
            }

            listNotifications->addItem(item);
        }
    }
}

void InfoWidget::onNotificationClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    QString content = item->data(Qt::UserRole + 1).toString();

    QSqlQuery q;
    q.prepare("UPDATE notifications SET read = 1 WHERE id = ? AND type = 'notification'");
    q.addBindValue(id);
    q.exec();

    for (auto &n : notifications) {
        if (n.id == id) {
            n.read = true;
            break;
        }
    }

    QMessageBox::information(this, "通知详情", content);
    refreshAll(); // 刷新以更新状态
}

void InfoWidget::onTodoClicked(int row, int col)
{
    if (col == 3) return; // 删除按钮列

    if (col == 2) {
        QTableWidgetItem *itemStatus = tableTodos->item(row, 2);
        int id = tableTodos->item(row, 0)->data(Qt::UserRole).toInt();
        bool completed = (itemStatus->text() == "已完成");

        QSqlQuery q;
        q.prepare("UPDATE notifications SET read = ? WHERE id = ? AND type = 'todo'");
        q.addBindValue(completed ? 0 : 1);
        q.addBindValue(id);
        if (q.exec()) {
            itemStatus->setText(completed ? "未完成" : "已完成");
            // 状态文字颜色
            itemStatus->setForeground(completed ? QBrush(QColor("#e6a23c")) : QBrush(QColor("#67c23a"))); // 黄色/绿色
        }
    } else {
        QString title = tableTodos->item(row, 0)->text();
        QString content = tableTodos->item(row, 0)->data(Qt::UserRole + 1).toString();
        QString deadline = tableTodos->item(row, 1)->text();
        QMessageBox::information(this, "待办详情",
                                 QString("标题：%1\n\n内容：%2\n\n截止日期：%3")
                                     .arg(title).arg(content).arg(deadline));
    }
}

void InfoWidget::clearRead()
{
    QSqlQuery q;
    q.exec("DELETE FROM notifications WHERE type='notification' AND read=1");
    refreshAll();
    QMessageBox::information(this, "成功", "已清空所有已读通知！");
}

void InfoWidget::markAllRead()
{
    QSqlQuery q;
    q.exec("UPDATE notifications SET read=1 WHERE type='notification'");
    for (auto &n : notifications) {
        n.read = true;
    }
    refreshAll();
    QMessageBox::information(this, "成功", "所有通知已标记为已读！");
}

void InfoWidget::loadNotifications()
{
    notifications.clear();
    QSqlQuery q("SELECT id,title,content,time,read,source FROM notifications WHERE type='notification' ORDER BY time DESC");
    while(q.next()) {
        Notification n;
        n.id = q.value(0).toInt();
        n.title = q.value(1).toString();
        n.content = q.value(2).toString();
        n.time = QDateTime::fromString(q.value(3).toString(), "yyyy-MM-dd HH:mm:ss");
        n.read = q.value(4).toBool();
        n.source = q.value(5).toString();
        n.type = "notification";
        notifications.append(n);
    }
}

void InfoWidget::loadAnnouncements()
{
    announcements.clear();
    listAnnouncements->clear();
    QSqlQuery q("SELECT id,title,content,time,isTop FROM notifications WHERE type='announcement' ORDER BY isTop DESC, time DESC");
    while(q.next()) {
        Notification a;
        a.id = q.value(0).toInt();
        a.title = q.value(1).toString();
        a.content = q.value(2).toString();
        a.time = QDateTime::fromString(q.value(3).toString(), "yyyy-MM-dd HH:mm:ss");
        a.isTop = q.value(4).toBool();
        a.type = "announcement";
        announcements.append(a);

        QString text = a.isTop ? "[置顶] " : "";
        text += a.title + "   " + a.time.toString("MM-dd HH:mm");
        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, a.id);
        item->setData(Qt::UserRole + 1, a.isTop);
        item->setData(Qt::UserRole + 2, a.content);

        if (a.isTop) {
            item->setForeground(QColor("#f56c6c")); // 红色
            QFont f = item->font(); f.setBold(true); item->setFont(f);
        } else {
            item->setForeground(QColor("#333333")); // 强制深色
        }
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        listAnnouncements->addItem(item);
    }
}

void InfoWidget::loadTodos()
{
    todos.clear();
    tableTodos->setRowCount(0);
    tableTodos->blockSignals(true);

    QSqlQuery q("SELECT id,title,content,deadline,read FROM notifications WHERE type='todo' ORDER BY deadline ASC");
    while(q.next()) {
        Notification t;
        t.id = q.value(0).toInt();
        t.title = q.value(1).toString();
        t.content = q.value(2).toString();
        t.deadline = q.value(3).toString();
        t.read = q.value(4).toBool();
        t.type = "todo";
        todos.append(t);

        int row = tableTodos->rowCount();
        tableTodos->insertRow(row);

        QTableWidgetItem *itemTitle = new QTableWidgetItem(t.title);
        itemTitle->setData(Qt::UserRole, t.id);
        itemTitle->setData(Qt::UserRole + 1, t.content);
        itemTitle->setForeground(QColor("#333333")); // 强制深色
        tableTodos->setItem(row, 0, itemTitle);

        QTableWidgetItem *itemDeadline = new QTableWidgetItem(t.deadline);
        QDateTime deadline = QDateTime::fromString(t.deadline, "yyyy-MM-dd HH:mm");
        // 日期颜色逻辑
        if (deadline < QDateTime::currentDateTime() && !t.read) {
            itemDeadline->setForeground(QColor("#f56c6c")); // 过期红
        } else if (deadline.secsTo(QDateTime::currentDateTime()) <= 86400 && !t.read) {
            itemDeadline->setForeground(QColor("#e6a23c")); // 临期橙
        } else {
            itemDeadline->setForeground(QColor("#333333")); // 正常黑
        }
        tableTodos->setItem(row, 1, itemDeadline);

        QTableWidgetItem *itemStatus = new QTableWidgetItem(t.read ? "已完成" : "未完成");
        // 状态文字颜色：完成绿，未完成橙
        itemStatus->setForeground(t.read ? QColor("#67c23a") : QColor("#e6a23c"));
        tableTodos->setItem(row, 2, itemStatus);

        QPushButton *btnDelete = new QPushButton("删除");
        btnDelete->setObjectName("DeleteBtn");
        btnDelete->setCursor(Qt::PointingHandCursor);

        connect(btnDelete, &QPushButton::clicked, this, [this, row]() {
            int id = tableTodos->item(row, 0)->data(Qt::UserRole).toInt();
            deleteTodo(id);
        });
        tableTodos->setCellWidget(row, 3, btnDelete);
    }
    tableTodos->blockSignals(false);
}

void InfoWidget::deleteTodo(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM notifications WHERE id = ? AND type = 'todo'");
    q.addBindValue(id);
    if (q.exec()) {
        refreshAll();
    } else {
        QMessageBox::critical(this, "错误", "删除失败：" + q.lastError().text());
    }
}

void InfoWidget::deleteAnnouncement(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM notifications WHERE id = ? AND type = 'announcement'");
    q.addBindValue(id);
    if (q.exec()) {
        refreshAll();
    } else {
        QMessageBox::critical(this, "错误", "删除失败：" + q.lastError().text());
    }
}

void InfoWidget::toggleAnnouncementTop(int id, bool isTop)
{
    QSqlQuery q;
    q.prepare("UPDATE notifications SET isTop = ? WHERE id = ? AND type = 'announcement'");
    q.addBindValue(isTop ? 1 : 0);
    q.addBindValue(id);

    if (q.exec()) {
        refreshAll();
    }
}

void InfoWidget::viewAnnouncementDetail(int id)
{
    for (const auto &a : announcements) {
        if (a.id == id) {
            QDialog dlg(this);
            dlg.setWindowTitle("公告详情");
            dlg.setStyleSheet("background: white;");
            dlg.setFixedSize(400, 300); // 固定大小更整洁

            QVBoxLayout *layout = new QVBoxLayout(&dlg);

            QLabel *lblTitle = new QLabel(a.title);
            lblTitle->setStyleSheet("font-size:18px; font-weight:bold; color: #2c3e50;");
            lblTitle->setWordWrap(true);

            QLabel *lblTime = new QLabel("发布时间：" + a.time.toString("yyyy-MM-dd HH:mm"));
            lblTime->setStyleSheet("color: #999; font-size: 12px; margin-bottom: 10px;");

            QTextEdit *txtContent = new QTextEdit(a.content);
            txtContent->setReadOnly(true);
            txtContent->setStyleSheet("border: none; background: #f9f9f9; padding: 10px; border-radius: 4px; color: #333;");

            layout->addWidget(lblTitle);
            layout->addWidget(lblTime);
            layout->addWidget(txtContent);

            QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Close);
            connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
            layout->addWidget(btns);

            dlg.exec();
            break;
        }
    }
}

void InfoWidget::onAnnouncementClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    viewAnnouncementDetail(id);
}

void InfoWidget::addNotification()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加系统通知");
    dlg.setStyleSheet("background: white;");

    QFormLayout *form = new QFormLayout(&dlg);

    QLineEdit *edtTitle = new QLineEdit;
    edtTitle->setStyleSheet("color: #333;");
    QTextEdit *edtContent = new QTextEdit;
    edtContent->setStyleSheet("color: #333;");
    QComboBox *edtSource = new QComboBox;
    edtSource->addItems({"学生管理","数据分析","数据导出"});

    form->addRow("标题:", edtTitle);
    form->addRow("内容:", edtContent);
    form->addRow("来源模块:", edtSource);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        if (edtTitle->text().isEmpty() || edtContent->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "警告", "标题和内容不能为空！");
            return;
        }
        QSqlQuery q;
        q.prepare("INSERT INTO notifications (title,content,time,source,type) VALUES (?,?,?,?,?)");
        q.addBindValue(edtTitle->text());
        q.addBindValue(edtContent->toPlainText());
        q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        q.addBindValue(edtSource->currentText());
        q.addBindValue("notification");
        if (q.exec()) {
            refreshAll();
        }
    }
}

void InfoWidget::addAnnouncement()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加公告");
    dlg.setStyleSheet("background: white;");
    QFormLayout *form = new QFormLayout(&dlg);

    QLineEdit *edtTitle = new QLineEdit;
    edtTitle->setStyleSheet("color: #333;");

    QTextEdit *edtContent = new QTextEdit;
    edtContent->setStyleSheet("color: #333;");
    edtContent->setMinimumHeight(100);

    QCheckBox *chkTop = new QCheckBox("设为置顶公告", &dlg);
    chkTop->setChecked(false);

    // 【核心修复】纯代码绘制复选框状态
    // 未选中：白底灰色边框
    // 选中：蓝色实心背景 (Blue Box)
    chkTop->setStyleSheet(R"(
        QCheckBox {
            color: #333333;
            spacing: 5px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 1px solid #999999;
            border-radius: 3px;
            background: white;
        }
        QCheckBox::indicator:hover {
            border-color: #3498db;
        }
        QCheckBox::indicator:checked {
            background-color: #3498db;
            border: 1px solid #3498db;
        }
    )");

    form->addRow("标题:", edtTitle);
    form->addRow("内容:", edtContent);
    // 直接放在右侧，避免Label点击问题
    form->addRow("", chkTop);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        if (edtTitle->text().isEmpty() || edtContent->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "警告", "标题和内容不能为空！");
            return;
        }
        QSqlQuery q;
        q.prepare("INSERT INTO notifications (title,content,time,isTop,type) VALUES (?,?,?,?,?)");
        q.addBindValue(edtTitle->text());
        q.addBindValue(edtContent->toPlainText());
        q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        q.addBindValue(chkTop->isChecked() ? 1 : 0);
        q.addBindValue("announcement");
        if (q.exec()) {
            refreshAll();
        }
    }
}

void InfoWidget::addTodo()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加待办");
    dlg.setStyleSheet("background: white;");
    QFormLayout *form = new QFormLayout(&dlg);

    QLineEdit *edtTitle = new QLineEdit;
    edtTitle->setStyleSheet("color: #333;");
    QTextEdit *edtContent = new QTextEdit;
    edtContent->setStyleSheet("color: #333;");
    QDateTimeEdit *edtDeadline = new QDateTimeEdit(QDateTime::currentDateTime().addDays(1));
    edtDeadline->setDisplayFormat("yyyy-MM-dd HH:mm");
    edtDeadline->setStyleSheet("color: #333;");

    form->addRow("标题:", edtTitle);
    form->addRow("内容:", edtContent);
    form->addRow("截止日期:", edtDeadline);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addWidget(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        if (edtTitle->text().isEmpty()) {
            QMessageBox::warning(this, "警告", "标题不能为空！");
            return;
        }
        QSqlQuery q;
        q.prepare("INSERT INTO notifications (title,content,deadline,type) VALUES (?,?,?,?)");
        q.addBindValue(edtTitle->text());
        q.addBindValue(edtContent->toPlainText());
        q.addBindValue(edtDeadline->dateTime().toString("yyyy-MM-dd HH:mm"));
        q.addBindValue("todo");
        if (q.exec()) {
            refreshAll();
        }
    }
}
