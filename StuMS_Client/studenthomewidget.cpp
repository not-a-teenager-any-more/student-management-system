#include "studenthomewidget.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QDate>
#include <QGraphicsDropShadowEffect> // 用于添加阴影效果

StudentHomeWidget::StudentHomeWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void StudentHomeWidget::setStudentInfo(const QString &username)
{
    // 更新欢迎语中的账号
    m_lblWelcome->setText(QString("👋 欢迎回来，同学 (账号: %1)").arg(username));

    // 获取当前日期并显示
    QString dateStr = QDate::currentDate().toString("yyyy年MM月dd日");
    m_lblAccount->setText(QString("📅 今天是：%1 | 状态：在校 (Active)").arg(dateStr));
}

void StudentHomeWidget::setupUi()
{
    // 1. 整体背景：淡雅的蓝灰色调
    this->setStyleSheet("QWidget { background-color: #f0f4f8; font-family: 'Microsoft YaHei'; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 50, 60, 50); // 四周留白
    mainLayout->setSpacing(30); // 控件间距

    // --- 2. 顶部标题栏 (中南大学元素) ---
    QWidget *headerContainer = new QWidget;
    headerContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerContainer);

    QLabel *title = new QLabel("🏫 中南大学 | Central South University");
    // 中南大学常用的深蓝色 #1e3a8a
    title->setStyleSheet("font-size: 36px; font-weight: bold; color: #1e3a8a; letter-spacing: 2px;");
    title->setAlignment(Qt::AlignCenter);

    QLabel *subTitle = new QLabel("智慧校园学生服务门户 · Student Service Portal");
    // 橄榄绿配色 #556b2f
    subTitle->setStyleSheet("font-size: 18px; color: #556b2f; margin-top: 5px;");
    subTitle->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(title);
    headerLayout->addWidget(subTitle);

    // --- 3. 个人欢迎卡片 ---
    QFrame *infoCard = new QFrame;
    infoCard->setStyleSheet(
        "QFrame { background-color: white; border-radius: 12px; border: 1px solid #d1d5db; }"
        );

    // 给卡片加一点阴影，更有立体感
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 4);
    infoCard->setGraphicsEffect(shadow);

    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(30, 20, 30, 20);

    m_lblWelcome = new QLabel("👋 欢迎回来，同学");
    m_lblWelcome->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; border: none;");
    m_lblWelcome->setAlignment(Qt::AlignCenter);

    m_lblAccount = new QLabel("加载中...");
    m_lblAccount->setStyleSheet("font-size: 16px; color: #6b7280; margin-top: 8px; border: none;");
    m_lblAccount->setAlignment(Qt::AlignCenter);

    infoLayout->addWidget(m_lblWelcome);
    infoLayout->addWidget(m_lblAccount);

    // --- 4. 静态公告栏 (写死的内容) ---
    QFrame *noticeBoard = new QFrame;
    noticeBoard->setStyleSheet(
        "QFrame { background-color: white; border-radius: 12px; border: 1px solid #d1d5db; }"
        );
    QVBoxLayout *noticeLayout = new QVBoxLayout(noticeBoard);

    QLabel *noticeHeader = new QLabel("📢 最新校园公告 / Campus Notice");
    noticeHeader->setStyleSheet("font-size: 18px; font-weight: bold; color: #d97706; border: none; padding-bottom: 10px; border-bottom: 2px solid #f3f4f6;");

    QTextEdit *content = new QTextEdit;
    content->setReadOnly(true); // 只读，不能编辑
    content->setStyleSheet("border: none; font-size: 15px; line-height: 28px; color: #374151; background: transparent;");

    // 🔥 这里是写死的公告内容 (HTML格式) 🔥
    content->setHtml(R"(
        <p style='margin-bottom:15px;'><b>[教务处] 关于2025-2026学年选课安排的通知</b><br>
        <span style='color:#999;'>2025-08-28</span> 全体本科生请注意，选课系统将于9月1日上午8:00开放...</p>

        <p style='margin-bottom:15px;'><b>[图书馆] 岳麓山校区图书馆开馆时间调整</b><br>
        <span style='color:#999;'>2025-08-25</span> 为满足考研同学需求，图书馆即日起闭馆时间延长至 23:30...</p>

        <p style='margin-bottom:15px;'><b>[科研动态] 我校粉末冶金研究院团队再创佳绩</b><br>
        <span style='color:#999;'>2025-08-20</span> 祝贺我校科研团队在顶级期刊发表关于高性能材料的最新研究成果！</p>

        <p style='margin-bottom:15px;'><b>[后勤服务] 暑期食堂装修暂停营业公告</b><br>
        <span style='color:#999;'>2025-08-15</span> 三食堂二楼将进行为期两周的升级改造，请移步一楼或四食堂就餐。</p>

        <hr style='border: 1px dashed #eee;'>
        <div style='text-align:center; color:#1e3a8a; margin-top:20px;'>
            <i>知行合一 · 经世致用</i>
        </div>
    )");

    noticeLayout->addWidget(noticeHeader);
    noticeLayout->addWidget(content);

    // --- 5. 组装所有部分 ---
    mainLayout->addWidget(headerContainer);
    mainLayout->addWidget(infoCard);
    mainLayout->addWidget(noticeBoard, 1); // 公告栏占据剩余空间，拉伸填充
}
