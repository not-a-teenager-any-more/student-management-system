#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDateTimeEdit>
#include <QMenu>
#include <QMessageBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QShortcut>

struct Notification {
    int id;
    QString title;
    QString content;
    QDateTime time;
    bool read;
    QString source;
    bool isTop;
    QString sender;
    QString deadline;
    QString type;
};

class InfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InfoWidget(QWidget *parent = nullptr);
    ~InfoWidget();

private slots:
    void refreshAll();
    void clearRead();
    void markAllRead();
    void filterSource(int index);
    void onNotificationClicked(QListWidgetItem *item);
    void onTodoClicked(int row, int col);
    void onAnnouncementClicked(QListWidgetItem *item);

    void addNotification();
    void addAnnouncement();
    void addTodo();
    void deleteTodo(int id);
    void deleteAnnouncement(int id);
    void toggleAnnouncementTop(int id, bool isTop);
    void viewAnnouncementDetail(int id);

    void switchToNotifications();
    void switchToTodos();
    void switchToAnnouncements();

    void showAnnouncementContextMenu(const QPoint &pos);

private:
    void setupUI();
    bool initDatabase();
    void loadNotifications();
    void loadAnnouncements();
    void loadTodos();
    void updateUnreadCount();

    QLabel *lblUnread;
    QComboBox *cmbSource;

    QListWidget *listNotifications;
    QListWidget *listAnnouncements;
    QTableWidget *tableTodos;

    QList<Notification> notifications;
    QList<Notification> announcements;
    QList<Notification> todos;

    QSqlDatabase db;

    QStackedWidget *stackedWidget;
};

#endif
