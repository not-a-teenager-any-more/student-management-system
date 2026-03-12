#include "exportworker.h"
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QThread>
#include "xlsxdocument.h"

ExportWorker::ExportWorker(ExportTask* task, QObject *parent)
    : QObject(parent), m_task(task)
{
    setAutoDelete(false); // 我们需要手动删除worker
}

void ExportWorker::run() {
    m_task->setStatus(ExportTask::Exporting);
    m_task->setProgress(0);

    try {
        QJsonArray students = m_task->studentsData();
        if (students.isEmpty()) {
            throw std::runtime_error("没有可导出的学生数据");
        }

        // 更新进度 (开始)
        m_task->setProgress(5);

        if (m_task->format() == "XLSX") {
            exportToXlsx(students);
        } else { // CSV
            exportToCsv(students);
        }

        // 更新进度 (完成)
        m_task->setProgress(100);
        m_task->setStatus(ExportTask::Completed);
        emit finished();
    } catch (const std::exception& e) {
        m_task->setStatus(ExportTask::Failed);
        emit error(QString("导出失败: %1").arg(e.what()));
    }
}

void ExportWorker::exportToXlsx(const QJsonArray& students) {
    QXlsx::Document xlsx;

    // 添加表头
    xlsx.write(1, 1, "学号");
    xlsx.write(1, 2, "姓名");
    xlsx.write(1, 3, "性别");
    xlsx.write(1, 4, "年龄");
    xlsx.write(1, 5, "院系");
    xlsx.write(1, 6, "专业");
    xlsx.write(1, 7, "班级");
    xlsx.write(1, 8, "电话");
    xlsx.write(1, 9, "邮箱");

    // 设置表头样式
    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setFontSize(12);
    headerFormat.setFillPattern(QXlsx::Format::PatternSolid);
    headerFormat.setPatternBackgroundColor(QColor(220, 220, 220));
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    xlsx.setRowFormat(1, headerFormat);

    // 添加数据行
    for (int i = 0; i < students.size(); ++i) {
        QJsonObject student = students.at(i).toObject();

        xlsx.write(i+2, 1, student.value("student_id").toString());
        xlsx.write(i+2, 2, student.value("name").toString());
        xlsx.write(i+2, 3, student.value("gender").toString());
        xlsx.write(i+2, 4, student.value("age").toInt());
        xlsx.write(i+2, 5, student.value("department").toString());
        xlsx.write(i+2, 6, student.value("major").toString());
        xlsx.write(i+2, 7, student.value("class").toString());
        xlsx.write(i+2, 8, student.value("phone").toString());
        xlsx.write(i+2, 9, student.value("email").toString());

        // 更新进度 (每处理一行更新一次)
        int progress = 5 + (i * 90 / students.size());
        m_task->setProgress(progress);

        // 检查是否被取消
        if (QThread::currentThread()->isInterruptionRequested()) {
            throw std::runtime_error("导出被取消");
        }
    }

    // 调整列宽
    for (int col = 1; col <= 9; ++col) {
        xlsx.setColumnWidth(col, 15);
    }

    // 保存文件
    if (!xlsx.saveAs(m_task->filePath())) {
        throw std::runtime_error("无法保存XLSX文件");
    }
}

void ExportWorker::exportToCsv(const QJsonArray& students) {
    QFile file(m_task->filePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("无法创建CSV文件");
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // 写入UTF-8 BOM头，确保Excel正确识别编码
    out << "\xEF\xBB\xBF";

    // 写入表头
    out << "学号,姓名,性别,年龄,院系,专业,班级,电话,邮箱\n";

    // 写入数据行
    for (int i = 0; i < students.size(); ++i) {
        QJsonObject student = students.at(i).toObject();

        out << "\"" << student.value("student_id").toString() << "\","
            << "\"" << student.value("name").toString() << "\","
            << "\"" << student.value("gender").toString() << "\","
            << student.value("age").toInt() << ","
            << "\"" << student.value("department").toString() << "\","
            << "\"" << student.value("major").toString() << "\","
            << "\"" << student.value("class").toString() << "\","
            << "\"" << student.value("phone").toString() << "\","
            << "\"" << student.value("email").toString() << "\"\n";

        // 更新进度 (每处理一行更新一次)
        int progress = 5 + (i * 90 / students.size());
        m_task->setProgress(progress);

        // 检查是否被取消
        if (QThread::currentThread()->isInterruptionRequested()) {
            file.close();
            file.remove(); // 删除未完成的文件
            throw std::runtime_error("导出被取消");
        }
    }

    file.close();
}
