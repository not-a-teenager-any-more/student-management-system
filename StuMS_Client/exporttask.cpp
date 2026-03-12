#include "exporttask.h"
#include <QDir>
#include <QDebug>

ExportTask::ExportTask(const QString& baseName, const QString& format,
                       const QString& path, QObject *parent)
    : QObject(parent), m_baseName(baseName), m_format(format),
    m_path(path), m_status(Pending), m_progress(0)
{
}

// --- 新增方法的实现 ---

void ExportTask::setExportPath(const QString& path) {
    // 更新导出路径
    m_path = path;
}

QString ExportTask::exportPath() const {
    return m_path;
}

// --- 原有方法的实现 ---

QString ExportTask::fileName() const {
    // 确保格式后缀是小写的，例如 .csv 或 .xlsx
    return QString("%1.%2").arg(m_baseName).arg(m_format.toLower());
}

void ExportTask::setBaseName(const QString& name) {
    if (m_baseName != name) {
        m_baseName = name;
        // 名字变了，可能会影响文件路径显示，这里不需要发信号，由Model处理刷新
    }
}

QString ExportTask::filePath() const {
    // 使用 QDir 组合路径和文件名，处理分隔符
    return QDir(m_path).filePath(fileName());
}

QString ExportTask::format() const {
    return m_format;
}

ExportTask::Status ExportTask::status() const {
    return m_status;
}

int ExportTask::progress() const {
    return m_progress;
}

QString ExportTask::statusText() const {
    switch(m_status) {
    case Pending: return "等待中";
    case Exporting: return QString("导出中(%1%)").arg(m_progress);
    case Completed: return "导出完成";
    case Failed: return "导出失败";
    default: return "未知状态";
    }
}

void ExportTask::setProgress(int progress) {
    if (m_progress != progress) {
        m_progress = progress;
        emit progressChanged(progress);
    }
}

void ExportTask::setStatus(Status status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

QJsonArray ExportTask::studentsData() const {
    return m_studentsData;
}

void ExportTask::setStudentsData(const QJsonArray& data) {
    m_studentsData = data;
}
