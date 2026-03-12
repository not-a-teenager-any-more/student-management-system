#include "exporttaskmodel.h"
#include <QApplication>
#include <QStyle>
#include <QBrush>
#include <QFont>
#include <QColor>

ExportTaskModel::ExportTaskModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int ExportTaskModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_tasks.size();
}

int ExportTaskModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 5; // 序号, 文件名, 格式, 状态, 操作
}

// --- 1. 允许文件名列被编辑 ---
Qt::ItemFlags ExportTaskModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    // 第1列(文件名)，且任务未完成/失败时，允许重命名
    if (index.column() == 1) {
        ExportTask* task = m_tasks.at(index.row());
        if (task->status() == ExportTask::Pending || task->status() == ExportTask::Failed) {
            flags |= Qt::ItemIsEditable;
        }
    }
    return flags;
}

// --- 2. 处理重命名 ---
bool ExportTaskModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole && index.column() == 1) {
        ExportTask* task = m_tasks.at(index.row());
        QString newName = value.toString().trimmed();
        if (!newName.isEmpty()) {
            // 注意：ExportTask 需要有 setBaseName 方法
            task->setBaseName(newName);
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

// --- 3. 核心显示逻辑 ---
QVariant ExportTaskModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_tasks.size())
        return QVariant();

    ExportTask* task = m_tasks.at(index.row());
    bool isFinished = (task->status() == ExportTask::Completed);

    // 图标装饰
    if (role == Qt::DecorationRole && index.column() == 1) {
        switch(task->status()) {
        case ExportTask::Completed: return QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton);
        case ExportTask::Failed:    return QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
        case ExportTask::Exporting: return QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
        default: return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
        }
    }

    // 文本显示
    if (role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0: return index.row() + 1;
        case 1: return task->fileName(); // 显示完整文件名(含后缀)
        case 2: return task->format();
        case 3: return task->statusText();
        case 4: return "移除";
        }
    }

    // **关键：编辑模式下只返回文件名部分，不带后缀**
    // 这样用户重命名时，直接全选的就是文件名，不会误删后缀
    if (role == Qt::EditRole && index.column() == 1) {
        QString name = task->fileName();
        int dotIdx = name.lastIndexOf('.');
        if (dotIdx != -1) return name.left(dotIdx);
        return name;
    }

    // 文字颜色：已完成变暗，状态列显示彩色
    if (role == Qt::ForegroundRole) {
        if (isFinished) {
            return QColor(Qt::gray); // 完成后整行变暗
        }
        if (index.column() == 3) { // 状态列彩色
            switch(task->status()) {
            case ExportTask::Failed:    return QColor("#D32F2F"); // 红色
            case ExportTask::Exporting: return QColor("#1976D2"); // 蓝色
            case ExportTask::Pending:   return QColor("#F57C00"); // 橙色
            default: break;
            }
        }
    }

    // 字体加粗
    if (role == Qt::FontRole && index.column() == 3) {
        QFont font;
        font.setBold(true);
        return font;
    }

    // 对齐
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 1) return QVariant(Qt::AlignVCenter | Qt::AlignLeft);
        return QVariant(Qt::AlignCenter);
    }

    return QVariant();
}

QVariant ExportTaskModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        QStringList headers = {"序号", "文件名", "格式", "当前状态", "操作"};
        if (section < headers.size()) return headers[section];
    }
    return QVariant();
}

bool ExportTaskModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || row + count > m_tasks.size()) return false;
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        m_tasks.at(row + i)->deleteLater();
        m_tasks.removeAt(row + i);
    }
    endRemoveRows();
    return true;
}

void ExportTaskModel::addTask(ExportTask* task) {
    beginInsertRows(QModelIndex(), m_tasks.size(), m_tasks.size());
    m_tasks.append(task);
    connect(task, &ExportTask::progressChanged, this, &ExportTaskModel::onTaskProgressChanged);
    connect(task, &ExportTask::statusChanged, this, &ExportTaskModel::onTaskStatusChanged);
    endInsertRows();
}

QList<ExportTask*> ExportTaskModel::pendingTasks() const {
    QList<ExportTask*> pending;
    for (ExportTask* task : m_tasks) {
        if (task->status() == ExportTask::Pending) pending.append(task);
    }
    return pending;
}

ExportTask* ExportTaskModel::taskAt(int row) const {
    if (row >= 0 && row < m_tasks.size()) return m_tasks.at(row);
    return nullptr;
}

void ExportTaskModel::removeFinishedTasks() {
    for (int i = m_tasks.size() - 1; i >= 0; --i) {
        if (m_tasks[i]->status() == ExportTask::Completed) {
            beginRemoveRows(QModelIndex(), i, i);
            m_tasks.at(i)->deleteLater();
            m_tasks.removeAt(i);
            endRemoveRows();
        }
    }
}

void ExportTaskModel::onTaskProgressChanged() {
    ExportTask* task = qobject_cast<ExportTask*>(sender());
    if (task) {
        int row = m_tasks.indexOf(task);
        if (row != -1) emit dataChanged(index(row, 3), index(row, 3));
    }
}

void ExportTaskModel::onTaskStatusChanged() {
    ExportTask* task = qobject_cast<ExportTask*>(sender());
    if (task) {
        int row = m_tasks.indexOf(task);
        // 状态变了，整行刷新（因为可能涉及颜色变暗、图标变化）
        if (row != -1) emit dataChanged(index(row, 0), index(row, 4));
    }
}
