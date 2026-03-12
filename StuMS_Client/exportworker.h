#ifndef EXPORTWORKER_H
#define EXPORTWORKER_H

#include <QObject>
#include <QRunnable>
#include "exporttask.h"

class ExportWorker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ExportWorker(ExportTask* task, QObject *parent = nullptr);
    void run() override;

    void exportToCsv(const QJsonArray &students);
    void exportToXlsx(const QJsonArray &students);
signals:
    void finished();
    void error(const QString& err);

private:
    ExportTask* m_task;
};

#endif // EXPORTWORKER_H
