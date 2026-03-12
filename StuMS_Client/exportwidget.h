#ifndef EXPORTWIDGET_H
#define EXPORTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QThreadPool>
#include <QLabel>
#include <QFrame>
#include "exporttaskmodel.h"

class ExportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExportWidget(QWidget *parent = nullptr);
    ExportTaskModel* getModel() const;

private slots:
    // 界面交互槽
    void onBrowsePath();
    void onOpenDestDir();
    void onPathChanged(const QString &text);
    void onRemoveTask(const QModelIndex &index);
    void onExportAll();
    void onClearFinished();

    // 线程回调
    void onTaskFinished();
    void onTaskError(const QString &err);

    // --- 新增高级交互 ---
    void onTableDoubleClicked(const QModelIndex &index);
    void onShowContextMenu(const QPoint &pos); // 右键菜单
    void onRenameTask();     // 重命名
    void onExportSingle();   // 单独导出
    void onOpenFile();       // 打开文件

    void refreshDashboard(); // 刷新顶部彩色状态栏

private:
    void setupUi();

    // UI 组件
    QLineEdit *m_pathEdit;
    QPushButton *m_btnBrowse;
    QPushButton *m_btnOpenDir;

    QFrame *m_bannerFrame; // 状态栏背景
    QLabel *m_bannerLabel; // 状态栏文字(HTML)

    QTableView *m_taskTable;
    QPushButton *m_btnClearFinished;
    QPushButton *m_btnExport;

    // 数据组件
    ExportTaskModel *m_model;
    QThreadPool m_threadPool;
    QString m_exportPath;
};

#endif // EXPORTWIDGET_H
