#include "exportwidget.h"
#include "exportworker.h"          // 你的 Worker 类
#include "deletebuttondelegate.h"  // 你的代理类
#include "exporttask.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QStyle>
#include <QHeaderView>
#include <QMenu>
#include <QDebug>

ExportWidget::ExportWidget(QWidget *parent) : QWidget(parent)
{
    m_model = new ExportTaskModel(this);
    m_threadPool.setMaxThreadCount(4); // 适当调整并发数
    m_exportPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    setupUi();
    refreshDashboard(); // 初始化状态栏
}

ExportTaskModel* ExportWidget::getModel() const {
    return m_model;
}

void ExportWidget::setupUi() {
    // 1. 顶部路径栏
    m_pathEdit = new QLineEdit(m_exportPath);
    m_pathEdit->setReadOnly(true); // 设为只读，强制使用按钮选择，避免路径错误
    m_pathEdit->setPlaceholderText("请选择导出目录...");

    m_btnBrowse = new QPushButton(" 更改目录");
    m_btnBrowse->setIcon(style()->standardIcon(QStyle::SP_DirIcon));

    m_btnOpenDir = new QPushButton(" 打开文件夹");
    m_btnOpenDir->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(new QLabel("导出位置:"));
    topLayout->addWidget(m_pathEdit);
    topLayout->addWidget(m_btnBrowse);
    topLayout->addWidget(m_btnOpenDir);

    // 2. 状态横幅 (Dashboard)
    m_bannerFrame = new QFrame;
    m_bannerFrame->setFixedHeight(50);
    // 浅灰背景
    m_bannerFrame->setStyleSheet("QFrame { background-color: #F8F9FA; border: 1px solid #DEE2E6; border-radius: 6px; }");

    m_bannerLabel = new QLabel;
    m_bannerLabel->setAlignment(Qt::AlignCenter);
    QFont f = font(); f.setPointSize(11);
    m_bannerLabel->setFont(f);

    QHBoxLayout *bannerLayout = new QHBoxLayout(m_bannerFrame);
    bannerLayout->addWidget(m_bannerLabel);

    // 3. 表格列表
    m_taskTable = new QTableView;
    m_taskTable->setModel(m_model);
    m_taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_taskTable->setAlternatingRowColors(true);
    m_taskTable->setShowGrid(false);
    m_taskTable->setContextMenuPolicy(Qt::CustomContextMenu); // 允许右键菜单

    // 列宽调整
    QHeaderView *header = m_taskTable->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Fixed);
    m_taskTable->setColumnWidth(4, 70);
    m_taskTable->verticalHeader()->setVisible(false);

    // 设置删除按钮代理
    DeleteButtonDelegate *delDelegate = new DeleteButtonDelegate(this);
    m_taskTable->setItemDelegateForColumn(4, delDelegate);
    connect(delDelegate, &DeleteButtonDelegate::deleteClicked, this, &ExportWidget::onRemoveTask);

    // 4. 底部操作栏
    m_btnClearFinished = new QPushButton("清理已完成");
    m_btnExport = new QPushButton(" 开始全部导出");
    m_btnExport->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));
    m_btnExport->setMinimumHeight(38);
    m_btnExport->setStyleSheet("QPushButton { font-weight: bold; font-size: 13px; }");

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_btnClearFinished);
    bottomLayout->addWidget(m_btnExport);

    // 5. 整体布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_bannerFrame);
    mainLayout->addWidget(m_taskTable);
    mainLayout->addLayout(bottomLayout);

    // 6. 信号连接
    connect(m_btnBrowse, &QPushButton::clicked, this, &ExportWidget::onBrowsePath);
    connect(m_btnOpenDir, &QPushButton::clicked, this, &ExportWidget::onOpenDestDir);
    connect(m_pathEdit, &QLineEdit::textChanged, this, &ExportWidget::onPathChanged);
    connect(m_btnExport, &QPushButton::clicked, this, &ExportWidget::onExportAll);
    connect(m_btnClearFinished, &QPushButton::clicked, this, &ExportWidget::onClearFinished);

    // 交互连接
    connect(m_taskTable, &QTableView::doubleClicked, this, &ExportWidget::onTableDoubleClicked);
    connect(m_taskTable, &QTableView::customContextMenuRequested, this, &ExportWidget::onShowContextMenu);

    // **状态栏实时刷新**：连接模型的任何变动
    connect(m_model, &ExportTaskModel::dataChanged, this, &ExportWidget::refreshDashboard);
    connect(m_model, &ExportTaskModel::rowsInserted, this, &ExportWidget::refreshDashboard);
    connect(m_model, &ExportTaskModel::rowsRemoved, this, &ExportWidget::refreshDashboard);
}

// ---------------------------------------------------------
// 状态栏逻辑：使用 HTML 渲染彩色数字
// ---------------------------------------------------------
void ExportWidget::refreshDashboard() {
    int pending = 0, exporting = 0, completed = 0, failed = 0;
    int total = m_model->rowCount();

    if (total == 0) {
        m_bannerLabel->setText("<font color='#999'>列表为空，请添加任务</font>");
        return;
    }

    for (int i = 0; i < total; ++i) {
        ExportTask* t = m_model->taskAt(i);
        if (!t) continue;
        switch (t->status()) {
        case ExportTask::Pending: pending++; break;
        case ExportTask::Exporting: exporting++; break;
        case ExportTask::Completed: completed++; break;
        case ExportTask::Failed: failed++; break;
        }
    }

    // 显眼的数字颜色
    QString html = QString(
                       "待处理: <font color='#F57C00' size='5'><b>%1</b></font> &nbsp;&nbsp;&nbsp; "
                       "进行中: <font color='#1976D2' size='5'><b>%2</b></font> &nbsp;&nbsp;&nbsp; "
                       "已成功: <font color='#388E3C' size='5'><b>%3</b></font> &nbsp;&nbsp;&nbsp; "
                       "失败: <font color='#D32F2F' size='5'><b>%4</b></font>"
                       ).arg(pending).arg(exporting).arg(completed).arg(failed);

    m_bannerLabel->setText(html);
}

// ---------------------------------------------------------
// 右键菜单逻辑
// ---------------------------------------------------------
void ExportWidget::onShowContextMenu(const QPoint &pos) {
    QModelIndex index = m_taskTable->indexAt(pos);
    if (!index.isValid()) return;

    ExportTask* task = m_model->taskAt(index.row());
    if (!task) return;

    QMenu menu(this);

    // 1. 重命名 (仅未开始或失败的任务)
    if (task->status() == ExportTask::Pending || task->status() == ExportTask::Failed) {
        QAction *actRename = menu.addAction(style()->standardIcon(QStyle::SP_FileIcon), "重命名");
        connect(actRename, &QAction::triggered, this, &ExportWidget::onRenameTask);
    }

    // 2. 单独导出
    QAction *actSingle = menu.addAction(style()->standardIcon(QStyle::SP_ArrowRight), "仅导出此项");
    // 如果正在导出，禁用此选项
    actSingle->setEnabled(task->status() != ExportTask::Exporting);
    connect(actSingle, &QAction::triggered, this, &ExportWidget::onExportSingle);

    menu.addSeparator();

    // 3. 打开逻辑 (仅已完成的任务)
    if (task->status() == ExportTask::Completed) {
        menu.addAction("打开文件", this, &ExportWidget::onOpenFile);

        // **打开所在位置**：使用 task->exportPath()，确保打开的是该文件实际导出的目录
        QAction *actOpenDir = menu.addAction(style()->standardIcon(QStyle::SP_DirIcon), "打开所在位置");
        connect(actOpenDir, &QAction::triggered, [task](){
            QDesktopServices::openUrl(QUrl::fromLocalFile(task->exportPath()));
        });
    }

    menu.addSeparator();

    // 4. 删除
    menu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), "移除", [this, index](){
        onRemoveTask(index);
    });

    menu.exec(m_taskTable->viewport()->mapToGlobal(pos));
}

// ---------------------------------------------------------
// 功能实现
// ---------------------------------------------------------

void ExportWidget::onRenameTask() {
    QModelIndex index = m_taskTable->currentIndex();
    if (index.isValid()) {
        // 进入编辑模式 (Model中已设置为 EditRole 只返回文件名不含后缀)
        // QTableView 默认行为即为全选文本
        QModelIndex nameIdx = m_model->index(index.row(), 1);
        m_taskTable->setCurrentIndex(nameIdx);
        m_taskTable->edit(nameIdx);
    }
}

void ExportWidget::onExportSingle() {
    QModelIndex index = m_taskTable->currentIndex();
    if (!index.isValid()) return;

    ExportTask* task = m_model->taskAt(index.row());
    if (!task) return;

    // 更新该任务的路径为当前选择的路径 (记录导出位置)
    task->setExportPath(m_exportPath);

    ExportWorker* worker = new ExportWorker(task);
    connect(worker, &ExportWorker::finished, this, &ExportWidget::onTaskFinished);
    connect(worker, &ExportWorker::error, this, &ExportWidget::onTaskError);
    m_threadPool.start(worker);

    refreshDashboard(); // 状态变为蓝色(进行中)
}

void ExportWidget::onExportAll() {
    QList<ExportTask*> tasks = m_model->pendingTasks();
    if (tasks.isEmpty()) {
        QMessageBox::information(this, "提示", "没有待处理的任务");
        return;
    }

    QDir dir(m_exportPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "错误", "导出路径不存在");
        return;
    }

    for (ExportTask* task : tasks) {
        task->setExportPath(m_exportPath);
        ExportWorker* worker = new ExportWorker(task);
        connect(worker, &ExportWorker::finished, this, &ExportWidget::onTaskFinished);
        connect(worker, &ExportWorker::error, this, &ExportWidget::onTaskError);
        m_threadPool.start(worker);
    }
    refreshDashboard();
}

void ExportWidget::onTaskFinished() {
    // 任务状态改变会自动触发 Model 的 dataChanged，进而触发 refreshDashboard
}

void ExportWidget::onTaskError(const QString &err) {
    qDebug() << "Export Error:" << err;
    // 同样会触发刷新
}

void ExportWidget::onBrowsePath() {
    QString dir = QFileDialog::getExistingDirectory(this, "选择导出目录", m_exportPath);
    if (!dir.isEmpty()) {
        m_pathEdit->setText(dir);
    }
}

void ExportWidget::onPathChanged(const QString &text) {
    m_exportPath = text;
}

void ExportWidget::onOpenDestDir() {
    // 顶部按钮打开的是当前设置的全局目录
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_exportPath));
}

void ExportWidget::onRemoveTask(const QModelIndex &index) {
    if (index.isValid()) m_model->removeRow(index.row());
}

void ExportWidget::onTableDoubleClicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    ExportTask* task = m_model->taskAt(index.row());
    if (task && task->status() == ExportTask::Completed) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(task->filePath()));
    }
}

void ExportWidget::onOpenFile() {
    QModelIndex index = m_taskTable->currentIndex();
    if (index.isValid()) {
        ExportTask* task = m_model->taskAt(index.row());
        if (task) QDesktopServices::openUrl(QUrl::fromLocalFile(task->filePath()));
    }
}

void ExportWidget::onClearFinished() {
    m_model->removeFinishedTasks();
}
