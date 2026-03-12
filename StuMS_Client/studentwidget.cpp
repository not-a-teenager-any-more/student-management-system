#include "studentwidget.h"
#include "networkmanager.h"
#include "studentdialog.h"
#include "exporttaskmodel.h"
#include "exporttask.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>
#include <QFrame>
#include <QFontDatabase>

// 构造函数
StudentWidget::StudentWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();

    auto &net = NetworkManager::instance();

    // 1. 监听数据接收 -> 刷新表格
    connect(&net, &NetworkManager::studentsDataReceived, this, &StudentWidget::updateTable);

    // 2. 监听操作结果 -> 提示并刷新
    connect(&net, &NetworkManager::operationResult, this, [this](bool success, QString msg){
        if(success) {
            // 操作成功后自动刷新列表
            refreshData();
            qDebug() << "Operation Success:" << msg;
        } else {
            // 操作失败弹窗警告
            QMessageBox::warning(this, "操作失败", msg);
        }
    });
}

void StudentWidget::setExportModel(ExportTaskModel *model) {
    m_exportModel = model;
}

void StudentWidget::setupUi() {
    // 设置整体样式
    this->setStyleSheet(""
                        "QWidget {"
                        "    background-color: #f5f7fa;"
                        "    font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;"
                        "}"
                        "QLineEdit {"
                        "    padding: 8px 12px;"
                        "    border: 2px solid #d1d9e6;"
                        "    border-radius: 6px;"
                        "    background: white;"
                        "    font-size: 14px;"
                        "}"
                        "QLineEdit:focus {"
                        "    border-color: #4d90fe;"
                        "    background: #f8fdff;"
                        "}"
                        "QPushButton {"
                        "    padding: 8px 16px;"
                        "    border-radius: 6px;"
                        "    font-weight: 500;"
                        "    font-size: 14px;"
                        "    border: none;"
                        "    transition: all 0.2s;"
                        "}"
                        "QPushButton:hover {"
                        "    transform: translateY(-1px);"
                        "    box-shadow: 0 4px 8px rgba(0,0,0,0.1);"
                        "}"
                        "QPushButton:pressed {"
                        "    transform: translateY(0);"
                        "    box-shadow: none;"
                        "}"
                        "QTableView {"
                        "    background: white;"
                        "    border: 1px solid #e0e6ef;"
                        "    border-radius: 8px;"
                        "    gridline-color: #eef2f7;"
                        "    selection-background-color: #e3f2fd;"
                        "    selection-color: #1565c0;"
                        "}"
                        "QHeaderView::section {"
                        "    background-color: #5c6bc0;"
                        "    color: white;"
                        "    padding: 10px;"
                        "    border: none;"
                        "    font-weight: 600;"
                        "    font-size: 13px;"
                        "}"
                        "QHeaderView::section:first {"
                        "    border-top-left-radius: 7px;"
                        "}"
                        "QHeaderView::section:last {"
                        "    border-top-right-radius: 7px;"
                        "}"
                        "QComboBox {"
                        "    padding: 6px 12px;"
                        "    border: 2px solid #d1d9e6;"
                        "    border-radius: 6px;"
                        "    background: white;"
                        "    min-width: 100px;"
                        "}"
                        "QComboBox::drop-down {"
                        "    border: none;"
                        "}"
                        "QComboBox::down-arrow {"
                        "    image: url(:/icons/down_arrow.svg);"
                        "    width: 12px;"
                        "    height: 12px;"
                        "}"
                        "");

    // --- 1. 顶部工具栏 ---
    QFrame *toolbarFrame = new QFrame;
    toolbarFrame->setStyleSheet(""
                                "QFrame {"
                                "    background: white;"
                                "    border-radius: 8px;"
                                "    padding: 12px;"
                                "    border: 1px solid #e0e6ef;"
                                "}"
                                "");

    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("🔍 输入姓名或学号搜索...");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(300);

    m_btnSearch = new QPushButton("查询");
    m_btnSearch->setStyleSheet(""
                               "QPushButton {"
                               "    background-color: #4d90fe;"
                               "    color: white;"
                               "    min-width: 80px;"
                               "}"
                               "QPushButton:hover {"
                               "    background-color: #357ae8;"
                               "}"
                               "");

    // 🔥🔥🔥 AI 语音助手按钮 🔥🔥🔥
    m_btnAI = new QPushButton(" DeepSeek 语音助手");
    m_btnAI->setStyleSheet(""
                           "QPushButton {"
                           "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                           "        stop:0 #8a2be2, stop:1 #6a0dad);"
                           "    color: white;"
                           "    font-weight: bold;"
                           "    min-width: 180px;"
                           "}"
                           "QPushButton:hover {"
                           "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                           "        stop:0 #9b30ff, stop:1 #7b1fa2);"
                           "}"
                           "");

    // 操作按钮组
    m_btnAdd = new QPushButton("＋ 添加");
    m_btnAdd->setStyleSheet(""
                            "QPushButton {"
                            "    background-color: #4d90fe;"
                            "    color: white;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #357ae8;"
                            "}"
                            "");

    m_btnDel = new QPushButton("－ 删除");
    m_btnDel->setStyleSheet(""
                            "QPushButton {"
                            "    background-color: #4d90fe;"
                            "    color: white;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #357ae8;"
                            "}"
                            "");

    m_btnUpdate = new QPushButton("✏️ 更新");
    m_btnUpdate->setStyleSheet(""
                               "QPushButton {"
                               "    background-color: #4d90fe;"
                               "    color: white;"
                               "}"
                               "QPushButton:hover {"
                               "    background-color: #357ae8;"
                               "}"
                               "");

    QHBoxLayout *topLayout = new QHBoxLayout(toolbarFrame);
    topLayout->addWidget(m_searchEdit);
    topLayout->addWidget(m_btnSearch);
    topLayout->addSpacing(20);

    // AI 按钮容器
    QFrame *aiFrame = new QFrame;
    aiFrame->setStyleSheet(""
                           "QFrame {"
                           "    background: linear-gradient(135deg, #8a2be2 0%, #6a0dad 100%);"
                           "    border-radius: 6px;"
                           "    padding: 2px;"
                           "}"
                           "");
    QHBoxLayout *aiLayout = new QHBoxLayout(aiFrame);
    aiLayout->setContentsMargins(0, 0, 0, 0);
    aiLayout->addWidget(m_btnAI);
    topLayout->addWidget(aiFrame);

    topLayout->addStretch();

    // 操作按钮组容器
    QFrame *actionFrame = new QFrame;
    QHBoxLayout *actionLayout = new QHBoxLayout(actionFrame);
    actionLayout->setSpacing(8);
    actionLayout->addWidget(m_btnAdd);
    actionLayout->addWidget(m_btnDel);
    actionLayout->addWidget(m_btnUpdate);
    topLayout->addWidget(actionFrame);

    // --- 2. 中间表格 ---
    QFrame *tableFrame = new QFrame;
    tableFrame->setStyleSheet(""
                              "QFrame {"
                              "    background: white;"
                              "    border-radius: 8px;"
                              "    border: 1px solid #e0e6ef;"
                              "}"
                              "");

    QVBoxLayout *tableContainer = new QVBoxLayout(tableFrame);
    tableContainer->setContentsMargins(1, 1, 1, 1);

    m_tableView = new QTableView;
    m_model = new QStandardItemModel(this);

    QStringList headers = {"学号", "姓名", "性别", "年龄", "院系", "专业", "班级", "电话", "邮箱"};
    m_model->setHorizontalHeaderLabels(headers);

    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setStyleSheet(""
                               "QTableView {"
                               "    border: none;"
                               "    font-size: 13px;"
                               "}"
                               "QTableView::item {"
                               "    padding: 8px;"
                               "    border-bottom: 1px solid #f0f0f0;"
                               "}"
                               "QTableView::item:selected {"
                               "    background: #e3f2fd;"
                               "    color: #1565c0;"
                               "    font-weight: 500;"
                               "}"
                               "QTableView::item:hover {"
                               "    background: #f8fafd;"
                               "}"
                               "");

    tableContainer->addWidget(m_tableView);

    // --- 3. 底部导出栏 ---
    QFrame *bottomFrame = new QFrame;
    bottomFrame->setStyleSheet(""
                               "QFrame {"
                               "    background: white;"
                               "    border-radius: 8px;"
                               "    padding: 16px;"
                               "    border: 1px solid #e0e6ef;"
                               "}"
                               "");

    m_btnAddToExport = new QPushButton("📥 添加至导出列表");
    m_btnAddToExport->setStyleSheet(""
                                    "QPushButton {"
                                    "    background-color: #2196f3;"
                                    "    color: white;"
                                    "    padding: 8px 20px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "    background-color: #1976d2;"
                                    "}"
                                    "");

    m_comboFormat = new QComboBox;
    m_comboFormat->addItems({"XLSX", "CSV"});

    QLabel *formatLabel = new QLabel("导出格式：");
    formatLabel->setStyleSheet("font-weight: 500; color: #555;");

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomFrame);
    bottomLayout->addStretch();
    bottomLayout->addWidget(formatLabel);
    bottomLayout->addWidget(m_comboFormat);
    bottomLayout->addSpacing(20);
    bottomLayout->addWidget(m_btnAddToExport);

    // --- 4. 总布局 ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    // 标题
    QLabel *titleLabel = new QLabel("👨‍🎓 学生信息管理");
    titleLabel->setStyleSheet(""
                              "QLabel {"
                              "    font-size: 20px;"
                              "    font-weight: bold;"
                              "    color: #333;"
                              "    padding: 8px 0;"
                              "}"
                              "");
    mainLayout->addWidget(titleLabel);

    mainLayout->addWidget(toolbarFrame);
    mainLayout->addWidget(tableFrame, 1);
    mainLayout->addWidget(bottomFrame);

    // --- 5. 信号连接 ---
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &StudentWidget::onSearch);
    connect(m_btnSearch, &QPushButton::clicked, this, &StudentWidget::onSearch);
    connect(m_btnAdd, &QPushButton::clicked, this, &StudentWidget::onAdd);
    connect(m_btnDel, &QPushButton::clicked, this, &StudentWidget::onDelete);
    connect(m_btnUpdate, &QPushButton::clicked, this, &StudentWidget::onUpdate);
    connect(m_btnAddToExport, &QPushButton::clicked, this, &StudentWidget::onAddToExport);

    // 连接 AI 按钮
    connect(m_btnAI, &QPushButton::clicked, this, &StudentWidget::onOpenAI);
}

void StudentWidget::updatePermissions(const QString &role) {
    m_currentRole = role;
    bool isAdmin = (role == "admin");
    m_btnAdd->setEnabled(isAdmin);
    m_btnDel->setEnabled(isAdmin);
    m_btnUpdate->setEnabled(isAdmin);
    m_btnAddToExport->setEnabled(isAdmin);

    QString style = m_btnAdd->styleSheet();
    if (!isAdmin) {
        style += "QPushButton:disabled { background-color: #bdbdbd; color: #777; }";
        m_btnAdd->setStyleSheet(style);
        m_btnDel->setStyleSheet(style);
        m_btnUpdate->setStyleSheet(style);
        m_btnAddToExport->setStyleSheet(style);
        m_searchEdit->setPlaceholderText("🔒 学生只能查询自己的信息");
        m_searchEdit->setStyleSheet("QLineEdit { background: #f0f0f0; }");
    }
}


void StudentWidget::refreshData() {
    onSearch();
}

void StudentWidget::onSearch() {
    QJsonObject req;
    req["type"] = "query_students";

    QJsonObject dataObj;
    dataObj["keyword"] = m_searchEdit->text();
    dataObj["role"] = m_currentRole;
    req["data"] = dataObj;

    // 显式设置 limit 防止分页导致数据不全
    req["limit"] = 10000;
    req["page"] = 1;

    NetworkManager::instance().sendRequest(req);
}

void StudentWidget::updateTable(const QJsonArray &data) {
    m_model->removeRows(0, m_model->rowCount());

    for(const auto &val : data) {
        QJsonObject s = val.toObject();
        QList<QStandardItem*> items;

        // 辅助 lambda：创建一个带有 ToolTip 的单元格
        // 这样鼠标放上去就会显示完整内容，解决显示不全的问题
        auto createItem = [](const QString &text) -> QStandardItem* {
            QStandardItem *item = new QStandardItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            item->setToolTip(text); // 🔥 核心代码：设置鼠标悬停提示 🔥
            return item;
        };

        items << createItem(s["student_id"].toString());
        items << createItem(s["name"].toString());

        // 性别也可以居中
        items << createItem(s["gender"].toString());

        // 年龄转字符串
        items << createItem(QString::number(s["age"].toInt()));

        items << createItem(s["department"].toString());
        items << createItem(s["major"].toString());
        items << createItem(s["class"].toString());
        items << createItem(s["phone"].toString());

        // 邮箱通常最长，最需要 ToolTip
        items << createItem(s["email"].toString());

        m_model->appendRow(items);
    }

    // 💡 额外优化：根据内容自动调整列宽，但限制最小宽度
    // 注意：如果有大量数据，ResizeToContents 可能会稍微慢一点，但在 1000 条以内无感
    // 如果你不希望列宽变来变去，可以注释掉下面这行，只用上面的 ToolTip 即可
    // m_tableView->resizeColumnsToContents();
}

QJsonArray StudentWidget::getAllTableData() const {
    QJsonArray arr;
    for(int r = 0; r < m_model->rowCount(); ++r) {
        QJsonObject obj;
        // 注意：列索引必须和 headers 顺序一致
        obj["student_id"] = m_model->item(r, 0)->text();
        obj["name"]       = m_model->item(r, 1)->text();
        obj["gender"]     = m_model->item(r, 2)->text();
        obj["age"]        = m_model->item(r, 3)->text().toInt();
        obj["department"] = m_model->item(r, 4)->text();
        // 传递更多字段给 AI，方便它分析
        obj["major"]      = m_model->item(r, 5)->text();
        obj["class"]      = m_model->item(r, 6)->text();
        arr.append(obj);
    }
    return arr;
}

// 🔥🔥🔥 AI 功能入口 🔥🔥🔥
void StudentWidget::onOpenAI() {
    AIChatDialog *dialog = new AIChatDialog(this);

    // =======================================================
    // ⬇️⬇️⬇️ 请在此填入你的 API KEY (双 Key 模式) ⬇️⬇️⬇️
    // =======================================================
    dialog->setDeepSeekApiKey(""); // 大脑 (DeepSeek)
    dialog->setSttApiKey("");      // 耳朵/嘴巴 (硅基流动)
    // =======================================================

    dialog->setContextData(getAllTableData());
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    // 核心：连接 AI 指令 -> 本地执行函数
    connect(dialog, &AIChatDialog::functionCallRequested,
            this, &StudentWidget::handleAIFunction);

    dialog->show();
}

// 🔥🔥🔥 AI 业务逻辑处理核心 🔥🔥🔥
void StudentWidget::handleAIFunction(const QString &funcName, const QJsonObject &args) {
    auto &net = NetworkManager::instance();
    QJsonObject req;

    qDebug() << ">>> AI 执行指令:" << funcName << "参数:" << args;

    // --- 1. 查询 ---
    if (funcName == "query_students") {
        QString keyword = args["keyword"].toString();
        m_searchEdit->setText(keyword);
        onSearch(); // 复用现有查询
    }
    // --- 2. 删除 (支持 ID 或 姓名) ---
    else if (funcName == "delete_student") {
        QString id = args["student_id"].toString();
        QString name = args["name"].toString();

        if (!id.isEmpty()) {
            // AI 给了学号，直接删
            req["type"] = "delete_students";
            req["data"] = QJsonObject{ {"student_ids", QJsonArray{id}} };
            net.sendRequest(req);
        }
        else if (!name.isEmpty()) {
            // AI 只给了名字，去表格里查学号
            QString foundId;
            int count = 0;
            for(int r = 0; r < m_model->rowCount(); ++r) {
                if(m_model->item(r, 1)->text() == name) {
                    foundId = m_model->item(r, 0)->text();
                    count++;
                }
            }

            if (count == 1) {
                req["type"] = "delete_students";
                req["data"] = QJsonObject{ {"student_ids", QJsonArray{foundId}} };
                net.sendRequest(req);
                QMessageBox::information(this, "AI助手", QString("已为您删除学生: %1 (学号: %2)").arg(name, foundId));
            } else if (count > 1) {
                QMessageBox::warning(this, "AI助手", QString("发现 %1 个叫 %2 的学生，请提供具体学号。").arg(count).arg(name));
            } else {
                QMessageBox::warning(this, "AI助手", QString("未找到名为 %1 的学生。").arg(name));
            }
        }
    }
    // --- 3. 添加 (最复杂的逻辑) ---
    else if (funcName == "add_student") {
        req["type"] = "add_student";
        QJsonObject data = args;

        // 【自动补全学号】如果 AI 没给，随机生成一个 (防止主键冲突)
        if (!data.contains("student_id") || data["student_id"].toString().isEmpty()) {
            data["student_id"] = QString::number(QRandomGenerator::global()->bounded(20250000, 20259999));
        }

        // 【性别转换】数据库要求 male/female
        QString g = data["gender"].toString().trimmed().toLower();
        if (g == "男" || g == "male") {
            data["gender"] = "male";
        } else {
            data["gender"] = "female"; // 默认 female
        }

        // 【补全必填字段】防止数据库报错
        if (!data.contains("age")) data["age"] = 20;
        if (!data.contains("department")) data["department"] = "未分配学院";
        if (!data.contains("major")) data["major"] = "通用专业";
        if (!data.contains("class")) data["class"] = "24级1班";
        if (!data.contains("phone")) data["phone"] = "13800000000";
        if (!data.contains("email")) data["email"] = data["student_id"].toString() + "@school.com";

        req["data"] = data;
        net.sendRequest(req);
    }
    // --- 4. 更新 ---
    else if (funcName == "update_student") {
        QString id = args["student_id"].toString();
        if(!id.isEmpty()) {
            req["type"] = "update_student";
            QJsonObject updateData = args;
            // 确保 updateData 里必须包含 student_id
            updateData["student_id"] = id;
            req["data"] = updateData;
            net.sendRequest(req);
        }
    }
}

// --- 以下为手动操作函数 (保持原样) ---

void StudentWidget::onAdd() {
    // ==========================================
    // 🎨 样式定义 (与 onUpdate 保持一致)
    // ==========================================
    QString dialogStyle = R"(
        QDialog { background-color: white; }
        QLabel { font-size: 13px; color: #5f6368; font-weight: 500; }

        QLineEdit, QComboBox, QSpinBox {
            border: 1px solid #dadce0; border-radius: 6px; padding: 8px; font-size: 14px;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
            border: 2px solid #1a73e8; padding: 7px;
        }

        /* 默认主按钮：深蓝色 */
        QPushButton {
            background-color: #1a73e8;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 24px;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover { background-color: #1557b0; }

        /* 次要按钮：白色 */
        QPushButton[text="Cancel"], QPushButton[text="&Cancel"],
        QPushButton[text="取消"], QPushButton[text="关闭"] {
            background-color: #ffffff;
            color: #5f6368;
            border: 1px solid #dcdfe6;
        }
        QPushButton[text="Cancel"]:hover, QPushButton[text="取消"]:hover {
            background-color: #f4f6f8;
            color: #1a73e8;
            border-color: #1a73e8;
        }
    )";

    // ==========================================
    // 🚀 初始化添加窗口
    // ==========================================
    StudentDialog dialog(this, false);
    dialog.setWindowTitle("✨ 添加新学生");
    dialog.setStyleSheet(dialogStyle); // 应用样式

    // UX: 自动聚焦第一个输入框
    QList<QLineEdit*> edits = dialog.findChildren<QLineEdit*>();
    if (!edits.isEmpty()) {
        edits[0]->setPlaceholderText("请输入学号");
        if(edits.size() > 1) edits[1]->setPlaceholderText("请输入姓名");
        edits[0]->setFocus();
    }

    if (dialog.exec() == QDialog::Accepted) {
        QJsonObject req;
        req["type"] = "add_student";
        req["data"] = QJsonObject {
            {"student_id", dialog.studentId()},
            {"name", dialog.name()},
            {"gender", dialog.gender()},
            {"age", dialog.age()},
            {"department", dialog.department()},
            {"major", dialog.major()},
            {"class", dialog.className()},
            {"phone", dialog.phone()},
            {"email", dialog.email()}
        };
        NetworkManager::instance().sendRequest(req);
    }
}

void StudentWidget::onDelete() {
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();

    // ==========================================
    // 🎨 定义删除弹窗的专属样式
    // ==========================================
    QString msgBoxStyle = R"(
        QMessageBox {
            background-color: white;
        }
        QMessageBox QLabel {
            color: #333;
            font-size: 14px;
            font-weight: 500;
        }

        /* 🔥 1. 默认按钮样式：高亮蓝 (用于“确定删除”按钮) 🔥 */
        QPushButton {
            background-color: #1a73e8; /* 谷歌蓝 */
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 20px;
            font-size: 13px;
            font-weight: bold;
            min-width: 90px;
        }
        QPushButton:hover {
            background-color: #1557b0; /* 悬停深蓝 */
        }
        QPushButton:pressed {
            background-color: #104386;
        }

        /* 🔥 2. 次要按钮样式：白色 (用于“取消”按钮) 🔥 */
        /* 匹配文字为 "No", "Cancel", "取消" 的按钮 */
        QPushButton[text="No"], QPushButton[text="Cancel"], QPushButton[text="取消"] {
            background-color: white;
            color: #5f6368;
            border: 1px solid #dcdfe6;
        }
        QPushButton[text="No"]:hover, QPushButton[text="Cancel"]:hover, QPushButton[text="取消"]:hover {
            background-color: #f4f6f8;
            border-color: #1a73e8;
            color: #1a73e8;
        }
    )";

    // ==========================================
    // ⚠️ 情况1：未选择学生
    // ==========================================
    if (selection.isEmpty()) {
        QMessageBox box(this);
        box.setWindowTitle("提示");
        box.setText("请先在列表中选择要删除的学生！");
        box.setIcon(QMessageBox::Warning);

        // 设置单按钮
        box.setStandardButtons(QMessageBox::Ok);
        box.button(QMessageBox::Ok)->setText("知道了");

        // 应用样式
        box.setStyleSheet(msgBoxStyle);
        box.exec();
        return;
    }

    // ==========================================
    // 🗑️ 情况2：确认删除弹窗
    // ==========================================
    QStringList ids;
    for(const auto &idx : selection) {
        ids << m_model->item(idx.row(), 0)->text();
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认删除");
    msgBox.setText(QString("⚠️ 确定要永久删除选中的 %1 名学生吗？\n此操作不可恢复。").arg(ids.size()));
    msgBox.setIcon(QMessageBox::Question);

    // 设置“是”和“否”按钮
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No); // 默认选中“取消”，防止手滑

    // 修改按钮文字，使其更直观
    msgBox.button(QMessageBox::Yes)->setText("确定删除");
    msgBox.button(QMessageBox::No)->setText("取消");

    // 🔥 应用高亮样式 🔥
    msgBox.setStyleSheet(msgBoxStyle);

    if (msgBox.exec() != QMessageBox::Yes) {
        return;
    }

    // 发送删除请求
    QJsonObject req;
    req["type"] = "delete_students";
    req["data"] = QJsonObject { {"student_ids", QJsonArray::fromStringList(ids)} };
    NetworkManager::instance().sendRequest(req);
}
void StudentWidget::onUpdate() {
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();

    // ==========================================
    // 🎨 1. 定义超强样式的 CSS
    // ==========================================

    // 针对 QMessageBox (警告框) 的样式
    QString msgBoxStyle = R"(
        QMessageBox { background-color: white; }
        QMessageBox QLabel { color: #333; font-size: 14px; }
        /* 强制让警告框里的按钮变蓝 */
        QMessageBox QPushButton {
            background-color: #1a73e8; color: white; border-radius: 4px;
            padding: 6px 24px; font-weight: bold; min-width: 70px; border: none;
        }
        QMessageBox QPushButton:hover { background-color: #1557b0; }
    )";

    // 针对 StudentDialog (编辑框) 的样式
    QString dialogStyle = R"(
        QDialog { background-color: white; }
        QLabel { font-size: 13px; color: #5f6368; font-weight: 500; }

        /* 输入框样式 */
        QLineEdit, QComboBox, QSpinBox {
            border: 1px solid #dadce0; border-radius: 6px; padding: 8px; font-size: 14px;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
            border: 2px solid #1a73e8; padding: 7px;
        }
        QLineEdit[readOnly="true"] {
            background-color: #f1f3f4; color: #80868b;
        }

        /* 🔥🔥🔥 按钮核心修改：默认全变蓝 (主色调) 🔥🔥🔥 */
        QPushButton {
            background-color: #1a73e8; /* 谷歌蓝 */
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 24px;
            font-size: 14px;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover { background-color: #1557b0; }
        QPushButton:pressed { background-color: #104386; }

        /* 🔥🔥🔥 仅仅把“取消”按钮变回白色 (次要色调) 🔥🔥🔥 */
        /* 匹配各种可能的取消文本，包括带快捷键 & 的情况 */
        QPushButton[text="Cancel"], QPushButton[text="&Cancel"],
        QPushButton[text="取消"], QPushButton[text="关闭"] {
            background-color: #ffffff;
            color: #5f6368;
            border: 1px solid #dcdfe6;
        }
        QPushButton[text="Cancel"]:hover, QPushButton[text="取消"]:hover {
            background-color: #f4f6f8;
            color: #1a73e8;
            border-color: #1a73e8;
        }
    )";

    // ==========================================
    // 🛡️ 2. 校验逻辑
    // ==========================================
    auto showWarning = [&](const QString &text) {
        QMessageBox box(this);
        box.setWindowTitle("提示");
        box.setText(text);
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Ok);
        box.button(QMessageBox::Ok)->setText("知道了");
        box.setStyleSheet(msgBoxStyle); // 应用警告框样式
        box.exec();
    };

    if (selection.isEmpty()) {
        showWarning("请先选择一位要修改的学生！");
        return;
    }

    if (selection.size() > 1) {
        showWarning("编辑功能不支持多选，请只选择一位学生。");
        return;
    }

    // ==========================================
    // 🚀 3. 打开编辑窗口
    // ==========================================
    int row = selection.first().row();
    StudentDialog dialog(this, true);

    QString currentId = m_model->item(row, 0)->text();
    QString currentName = m_model->item(row, 1)->text();

    dialog.setWindowTitle("✏️ 编辑 - " + currentName);
    dialog.setStyleSheet(dialogStyle); // 应用编辑框样式

    // 回填数据
    dialog.setStudentData(
        currentId,
        currentName,
        m_model->item(row, 2)->text(),
        m_model->item(row, 3)->text().toInt(),
        m_model->item(row, 4)->text(),
        m_model->item(row, 5)->text(),
        m_model->item(row, 6)->text(),
        m_model->item(row, 7)->text(),
        m_model->item(row, 8)->text()
        );

    // 锁定学号
    QList<QLineEdit*> edits = dialog.findChildren<QLineEdit*>();
    for(auto edit : edits) {
        if (edit->text() == currentId && !currentId.isEmpty()) {
            edit->setReadOnly(true);
            edit->setToolTip("学号不可修改");
            edit->setPlaceholderText("系统锁定");
        }
    }

    if (dialog.exec() == QDialog::Accepted) {
        QJsonObject req;
        req["type"] = "update_student";
        req["data"] = QJsonObject {
            {"student_id", currentId},
            {"name", dialog.name()},
            {"gender", dialog.gender()},
            {"age", dialog.age()},
            {"department", dialog.department()},
            {"major", dialog.major()},
            {"class", dialog.className()},
            {"phone", dialog.phone()},
            {"email", dialog.email()}
        };
        NetworkManager::instance().sendRequest(req);
    }
}

QJsonArray StudentWidget::getSelectedStudentsData() const {
    QJsonArray arr;
    QModelIndexList selection = m_tableView->selectionModel()->selectedRows();

    if (selection.isEmpty()) return arr;

    for(const QModelIndex &idx : selection) {
        int r = idx.row();
        QJsonObject obj;
        obj["student_id"] = m_model->item(r, 0)->text();
        obj["name"]       = m_model->item(r, 1)->text();
        obj["gender"]     = m_model->item(r, 2)->text();
        obj["age"]        = m_model->item(r, 3)->text().toInt();
        obj["department"] = m_model->item(r, 4)->text();
        obj["major"]      = m_model->item(r, 5)->text();
        obj["class"]      = m_model->item(r, 6)->text();
        obj["phone"]      = m_model->item(r, 7)->text();
        obj["email"]      = m_model->item(r, 8)->text();
        arr.append(obj);
    }
    return arr;
}

void StudentWidget::onAddToExport() {
    QJsonArray data = getSelectedStudentsData();

    if(data.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先在表格中选中要导出的学生！\n(按住 Ctrl 或 Shift 可多选)");
        return;
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString baseName = QString("selected_students_%1").arg(timeStr);
    QString format = m_comboFormat->currentText();

    ExportTask* task = new ExportTask(baseName, format, "", this);
    task->setStudentsData(data);
    m_exportModel->addTask(task);

    QMessageBox::information(this, "提示",
                             QString("已将选中的 %1 名学生添加到导出任务列表。\n请前往'数据导出'页面执行导出。").arg(data.size()));
}
