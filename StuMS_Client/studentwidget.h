#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QHeaderView>
#include <QMessageBox>

#include "exporttaskmodel.h"
#include "aichatdialog.h"

class StudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentWidget(QWidget *parent = nullptr);

    void setExportModel(ExportTaskModel *model);
    void updatePermissions(const QString &role);
    void refreshData();

private slots:
    void onSearch();
    void onAdd();
    void onDelete();
    void onUpdate();
    void onAddToExport();
    void updateTable(const QJsonArray &data);

    void onOpenAI();
    void handleAIFunction(const QString &funcName, const QJsonObject &args);

private:
    void setupUi();
    QJsonArray getSelectedStudentsData() const;
    QJsonArray getAllTableData() const;

    QLineEdit *m_searchEdit;
    QPushButton *m_btnSearch;
    QPushButton *m_btnAI;
    QPushButton *m_btnAdd;
    QPushButton *m_btnDel;
    QPushButton *m_btnUpdate;

    QTableView *m_tableView;
    QStandardItemModel *m_model;

    QPushButton *m_btnAddToExport;
    QComboBox *m_comboFormat;

    ExportTaskModel *m_exportModel = nullptr;
    QString m_currentRole;
};

#endif // STUDENTWIDGET_H
