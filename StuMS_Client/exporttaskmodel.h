#ifndef EXPORTTASKMODEL_H
#define EXPORTTASKMODEL_H

#include <QAbstractTableModel>
#include "exporttask.h"

class ExportTaskModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ExportTaskModel(QObject *parent = nullptr);

    // 基本模型接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // 核心数据展示与编辑
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // 业务接口
    void addTask(ExportTask* task);
    QList<ExportTask*> pendingTasks() const;
    ExportTask* taskAt(int row) const;
    void removeFinishedTasks();

private slots:
    void onTaskProgressChanged();
    void onTaskStatusChanged();

private:
    QList<ExportTask*> m_tasks;
};

#endif // EXPORTTASKMODEL_H
