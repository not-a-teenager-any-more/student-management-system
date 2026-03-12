#ifndef EXPORTTASK_H
#define EXPORTTASK_H

#include <QJsonArray>
#include <QObject>
#include <QString>

class ExportTask : public QObject
{
    Q_OBJECT
public:
    // 任务状态枚举
    enum Status {
        Pending,    // 等待中
        Exporting,  // 导出中
        Completed,  // 已完成
        Failed      // 已失败
    };
    Q_ENUM(Status)  // 注册枚举到Qt元对象系统

    // 构造函数
    explicit ExportTask(const QString& baseName, const QString& format,
                        const QString& path, QObject *parent = nullptr);

    // --- 新增/修改的方法 ---
    // 设置导出目录路径 (解决 setExportPath 报错)
    void setExportPath(const QString& path);
    // 获取导出目录路径
    QString exportPath() const;

      void setBaseName(const QString& name);

    // --- 原有方法 ---
    // 获取文件名（带扩展名）
    QString fileName() const;
    // 获取文件完整路径（目录 + 文件名）
    QString filePath() const;
    // 获取导出格式（CSV/XLSX）
    QString format() const;
    // 获取当前状态
    Status status() const;
    // 获取导出进度（0-100）
    int progress() const;
    // 获取状态描述文本
    QString statusText() const;

    // 设置进度值
    void setProgress(int progress);
    // 设置任务状态
    void setStatus(Status status);

    // 获取学生数据
    QJsonArray studentsData() const;
    // 设置学生数据
    void setStudentsData(const QJsonArray& data);

signals:
    // 进度变化信号
    void progressChanged(int progress);
    // 状态变化信号
    void statusChanged(Status status);

private:
    QString m_baseName;       // 文件名（不含扩展名）
    QString m_format;         // 文件格式（CSV/XLSX）
    QString m_path;           // 导出目录路径
    Status m_status;          // 当前状态
    int m_progress;           // 当前进度（0-100）
    QJsonArray m_studentsData; // 学生数据（JSON数组）
};

#endif // EXPORTTASK_H
