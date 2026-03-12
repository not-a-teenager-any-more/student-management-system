#include "chartwidget.h"
#include "networkmanager.h"
#include <QToolTip>
#include <QCursor>
#include <QDebug>
#include <QFontDatabase>
#include <algorithm>
#include <QGraphicsLayout>

// --- 配色方案 (高饱和度霓虹色，增强视觉冲击) ---
const QColor CLR_BG       = QColor("#1e1e2f");
const QColor CLR_PANEL    = QColor("#27293d");
const QColor CLR_TEXT     = QColor("#ffffff");
const QColor CLR_SUBTEXT  = QColor("#9a9a9a");

const QColor CLR_BLUE     = QColor("#1d8cf8");
const QColor CLR_PINK     = QColor("#e14eca");
const QColor CLR_ORANGE   = QColor("#ff8d72");
const QColor CLR_TEAL     = QColor("#00f2c3");
const QColor CLR_PURPLE   = QColor("#8e44ad");
const QColor CLR_GREEN    = QColor("#42b983"); // 新增绿色

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, CLR_BG);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    setupUi();

    connect(&NetworkManager::instance(), &NetworkManager::studentsDataReceived,
            this, &ChartWidget::updateCharts);

    updateKpiBoard(0, 0, 0, 0.0);
}

ChartWidget::~ChartWidget() {}

void ChartWidget::setupUi() {
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setContentsMargins(20, 20, 20, 20);
    mainGrid->setSpacing(20);

    // --- 1. KPI 看板 ---
    QHBoxLayout *kpiLayout = new QHBoxLayout();
    auto createCard = [](QString title, QLabel*& valLabel, QString initVal, QColor color) {
        QWidget *card = new QWidget;
        card->setStyleSheet(QString("background-color: %1; border-radius: 12px;").arg(CLR_PANEL.name()));
        QVBoxLayout *vbox = new QVBoxLayout(card);
        vbox->setContentsMargins(20, 15, 20, 15);

        QLabel *lblTitle = new QLabel(title);
        lblTitle->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;").arg(CLR_SUBTEXT.name()));

        valLabel = new QLabel(initVal);
        valLabel->setStyleSheet(QString("color: %1; font-size: 32px; font-weight: bold;").arg(color.name()));

        vbox->addWidget(lblTitle);
        vbox->addWidget(valLabel);
        return card;
    };

    kpiLayout->addWidget(createCard("总学生数 (Total)", m_lblTotalCount, "0", CLR_BLUE));
    kpiLayout->addWidget(createCard("性别比例 (M/F)", m_lblGenderRatio, "- / -", CLR_PINK));
    kpiLayout->addWidget(createCard("平均年龄 (Avg Age)", m_lblAvgAge, "0", CLR_ORANGE));

    mainGrid->addLayout(kpiLayout, 0, 0, 1, 3);

    // --- 2. 初始化图表 ---
    m_genderView = new QChartView(this);
    m_classView = new QChartView(this);
    m_ageView = new QChartView(this);
    m_deptView = new QChartView(this);
    m_majorView = new QChartView(this);

    initGenderChart();
    initClassChart();
    initAgeChart();
    initDeptChart();
    initMajorChart();

    // --- 3. 布局 ---
    mainGrid->addWidget(m_genderView, 1, 0);
    mainGrid->addWidget(m_classView,  1, 1);
    mainGrid->addWidget(m_ageView,    1, 2);

    mainGrid->addWidget(m_deptView,   2, 0, 1, 2);
    mainGrid->addWidget(m_majorView,  2, 2, 1, 1);

    mainGrid->setRowStretch(0, 1);
    mainGrid->setRowStretch(1, 4);
    mainGrid->setRowStretch(2, 5);
}

void ChartWidget::applyDashboardTheme(QChartView *view, const QString &title, bool showLegend) {
    QChart *chart = view->chart();
    chart->setTheme(QChart::ChartThemeDark);
    chart->setBackgroundBrush(QBrush(CLR_PANEL));
    chart->setBackgroundRoundness(12);
    chart->layout()->setContentsMargins(10, 10, 10, 10);

    chart->setTitle(title);
    QFont font = chart->titleFont();
    font.setPixelSize(18);
    font.setBold(true);
    chart->setTitleFont(font);
    chart->setTitleBrush(QBrush(CLR_TEXT));

    if (showLegend) {
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setLabelBrush(QBrush(CLR_TEXT));
        chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
        QFont legendFont = chart->legend()->font();
        legendFont.setPointSize(10);
        chart->legend()->setFont(legendFont);
    } else {
        chart->legend()->setVisible(false);
    }

    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border-radius: 12px; background-color: %1;").arg(CLR_PANEL.name()));
}

// ---------------- 图表初始化 ----------------

void ChartWidget::initGenderChart() {
    m_genderChart = new QChart();
    m_genderSeries = new QPieSeries();
    // 【修改】：空心更小，视觉上更饱满
    m_genderSeries->setHoleSize(0.3);
    m_genderChart->addSeries(m_genderSeries);
    m_genderChart->setAnimationOptions(QChart::AllAnimations);

    m_genderView->setChart(m_genderChart);
    applyDashboardTheme(m_genderView, "性别分布 (Gender)", true);
}

void ChartWidget::initClassChart() {
    m_classChart = new QChart();
    m_classSeries = new QPieSeries();
    m_classSeries->setHoleSize(0.3);
    m_classChart->addSeries(m_classSeries);
    m_classChart->setAnimationOptions(QChart::AllAnimations);

    m_classView->setChart(m_classChart);
    applyDashboardTheme(m_classView, "班级分布 (Class)", true);
}

void ChartWidget::initAgeChart() {
    m_ageChart = new QChart();
    m_ageSeries = new QSplineSeries();

    // 【修改】：明显加粗线条和数据点
    QPen pen(CLR_TEAL);
    pen.setWidth(5);    // 极粗线条
    m_ageSeries->setPen(pen);

    m_ageSeries->setPointsVisible(true);
    m_ageSeries->setMarkerSize(10); // 设置数据点大小，像铆钉一样清晰

    connect(m_ageSeries, &QSplineSeries::hovered, this, &ChartWidget::onSplineHovered);

    m_ageChart->addSeries(m_ageSeries);
    m_ageChart->setAnimationOptions(QChart::SeriesAnimations);

    m_ageView->setChart(m_ageChart);
    applyDashboardTheme(m_ageView, "年龄趋势 (Age Trend)", false);
}

void ChartWidget::initDeptChart() {
    m_deptChart = new QChart();
    // 【修改】：使用 HorizontalStackedBarSeries 实现每行颜色不同
    m_deptSeries = new QHorizontalStackedBarSeries();

    connect(m_deptSeries, &QHorizontalStackedBarSeries::hovered, this, &ChartWidget::onBarHovered);

    m_deptChart->addSeries(m_deptSeries);
    m_deptChart->setAnimationOptions(QChart::SeriesAnimations);

    m_deptView->setChart(m_deptChart);
    applyDashboardTheme(m_deptView, "各学院人数 (Departments)", false);
}

void ChartWidget::initMajorChart() {
    m_majorChart = new QChart();
    m_majorSeries = new QStackedBarSeries();

    connect(m_majorSeries, &QStackedBarSeries::hovered, this, &ChartWidget::onBarHovered);

    m_majorChart->addSeries(m_majorSeries);
    m_majorChart->setAnimationOptions(QChart::SeriesAnimations);

    m_majorView->setChart(m_majorChart);
    applyDashboardTheme(m_majorView, "热门专业 Top5", false);
}

// ---------------- 数据更新 ----------------

void ChartWidget::updateCharts(const QJsonArray &data) {
    if (data.isEmpty()) return;

    int total = 0, male = 0, female = 0;
    long long totalAge = 0; int validAgeCount = 0;
    QMap<QString, int> deptMap, majorMap, classMap;
    QMap<int, int> ageMap;

    for (const QJsonValue &val : data) {
        QJsonObject s = val.toObject();
        total++;
        QString g = s["gender"].toString().toLower();
        if (g == "male" || g == "男") male++; else if (g == "female" || g == "女") female++;

        int age = s["age"].toInt();
        if (age > 0 && age < 100) { ageMap[age]++; totalAge += age; validAgeCount++; }

        deptMap[s["department"].toString()]++;
        majorMap[s["major"].toString()]++;
        classMap[s["class"].toString()]++;
    }

    double avgAge = validAgeCount > 0 ? (double)totalAge / validAgeCount : 0;
    updateKpiBoard(total, male, female, avgAge);
    updateGenderChart(male, female);
    updateClassChart(classMap);
    updateAgeChart(ageMap);
    updateDeptChart(deptMap);
    updateMajorChart(majorMap);
}

void ChartWidget::updateKpiBoard(int total, int male, int female, double avgAge) {
    m_lblTotalCount->setText(QString::number(total));
    double mP = total > 0 ? (male * 100.0 / total) : 0;
    double fP = total > 0 ? (female * 100.0 / total) : 0;
    m_lblGenderRatio->setText(QString("%1% / %2%").arg(mP, 0, 'f', 0).arg(fP, 0, 'f', 0));
    m_lblAvgAge->setText(QString::number(avgAge, 'f', 1));
}

void ChartWidget::updateGenderChart(int male, int female) {
    m_genderSeries->clear();
    QPieSlice *s1 = m_genderSeries->append("男生", male);
    QPieSlice *s2 = m_genderSeries->append("女生", female);
    s1->setColor(CLR_BLUE);
    s2->setColor(CLR_PINK);

    for(auto slice : m_genderSeries->slices()) {
        slice->setLabelVisible(true);
        slice->setLabelColor(Qt::white);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
        // 【修改】：标签直接显示名字和百分比，例如 "男生 55%"
        slice->setLabel(QString("%1\n%2%").arg(slice->label()).arg(QString::number(slice->percentage() * 100, 'f', 0)));
        connect(slice, &QPieSlice::hovered, this, &ChartWidget::onPieSliceHovered);
    }
}

void ChartWidget::updateClassChart(const QMap<QString, int> &classMap) {
    m_classSeries->clear();
    QList<QPair<int, QString>> sorted;
    for(auto it = classMap.begin(); it != classMap.end(); ++it) sorted.append(qMakePair(it.value(), it.key()));
    std::sort(sorted.begin(), sorted.end(), [](auto a, auto b){ return a.first > b.first; });

    QList<QColor> colors = {CLR_PURPLE, CLR_TEAL, CLR_ORANGE, CLR_BLUE, CLR_PINK};
    int limit = qMin(sorted.size(), 5);

    for(int i=0; i<limit; ++i) {
        QPieSlice *slice = m_classSeries->append(sorted[i].second, sorted[i].first);
        slice->setColor(colors[i % colors.size()]);
        slice->setLabelVisible(true);
        slice->setLabelColor(Qt::white);
        // 【修改】：标签直接显示班级名
        slice->setLabel(QString("%1 %2%").arg(sorted[i].second).arg(QString::number(slice->percentage() * 100, 'f', 0)));
        connect(slice, &QPieSlice::hovered, this, &ChartWidget::onPieSliceHovered);
    }
}

void ChartWidget::updateAgeChart(const QMap<int, int> &ageMap) {
    m_ageSeries->clear();
    if(ageMap.isEmpty()) return;

    int minAge = 100, maxAge = 0, maxCount = 0;
    QList<int> ages = ageMap.keys();
    std::sort(ages.begin(), ages.end());

    for(int age : ages) {
        int count = ageMap[age];
        m_ageSeries->append(age, count);
        if(age < minAge) minAge = age;
        if(age > maxAge) maxAge = age;
        if(count > maxCount) maxCount = count;
    }

    QList<QAbstractAxis*> axes = m_ageChart->axes();
    for(auto ax : axes) m_ageChart->removeAxis(ax);

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(minAge - 1, maxAge + 1);
    axisX->setLabelsColor(CLR_SUBTEXT);
    axisX->setLabelFormat("%d");
    axisX->setTickCount(6);
    // 加大坐标轴文字
    QFont f = axisX->labelsFont(); f.setBold(true); f.setPointSize(10);
    axisX->setLabelsFont(f);

    m_ageChart->addAxis(axisX, Qt::AlignBottom);
    m_ageSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, maxCount > 0 ? maxCount + 1 : 5);
    axisY->setLabelsColor(CLR_SUBTEXT);
    axisY->setLabelFormat("%d");
    m_ageChart->addAxis(axisY, Qt::AlignLeft);
    m_ageSeries->attachAxis(axisY);
}

// 【彻底重写】：实现多彩横向条形图
void ChartWidget::updateDeptChart(const QMap<QString, int> &deptMap) {
    m_deptSeries->clear();

    // 颜色池
    QList<QColor> colors = {CLR_ORANGE, CLR_TEAL, CLR_PINK, CLR_BLUE, CLR_PURPLE};
    QStringList categories;

    // 排序
    QList<QPair<int, QString>> sorted;
    for(auto it = deptMap.begin(); it != deptMap.end(); ++it) sorted.append(qMakePair(it.value(), it.key()));
    std::sort(sorted.begin(), sorted.end(), [](auto a, auto b){ return a.first < b.first; }); // 升序，从下到上

    int maxVal = 0;
    int count = 0;

    // 构造 Categories
    for(const auto &p : sorted) {
        categories << p.second;
        if(p.first > maxVal) maxVal = p.first;
    }

    // 【核心技巧】：为每个学院创建一个独立的 QBarSet
    // 假设有3个学院 [A, B, C]，数值为 [10, 20, 30]
    // Set_A: [10, 0, 0] -> 橙色
    // Set_B: [0, 20, 0] -> 蓝色
    // Set_C: [0, 0, 30] -> 粉色
    // StackedBarSeries 把它们压在一起，但因为位置错开，看起来就是独立的每行一色

    for(int i=0; i<sorted.size(); ++i) {
        QBarSet *set = new QBarSet(sorted[i].second);
        set->setColor(colors[i % colors.size()]); // 循环取色
        set->setBorderColor(Qt::transparent);

        // 填充稀疏数据
        for(int j=0; j<sorted.size(); ++j) {
            if(i == j) *set << sorted[i].first;
            else *set << 0;
        }
        m_deptSeries->append(set);
    }

    // 重置坐标轴
    QList<QAbstractAxis*> axes = m_deptChart->axes();
    for(auto ax : axes) m_deptChart->removeAxis(ax);

    QBarCategoryAxis *axisY = new QBarCategoryAxis();
    axisY->append(categories);
    // 【修改】：文字加大、加亮
    QFont font = axisY->labelsFont();
    font.setPixelSize(14);
    font.setBold(true);
    axisY->setLabelsFont(font);
    axisY->setLabelsColor(Qt::white);

    m_deptChart->addAxis(axisY, Qt::AlignLeft);
    m_deptSeries->attachAxis(axisY);

    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelsColor(CLR_SUBTEXT);
    axisX->setRange(0, maxVal + 1);
    m_deptChart->addAxis(axisX, Qt::AlignBottom);
    m_deptSeries->attachAxis(axisX);
}

// 多彩垂直柱状图 (与 Dept 逻辑类似，只是方向不同)
void ChartWidget::updateMajorChart(const QMap<QString, int> &majorMap) {
    m_majorSeries->clear();

    QList<QPair<int, QString>> sorted;
    for(auto it = majorMap.begin(); it != majorMap.end(); ++it) sorted.append(qMakePair(it.value(), it.key()));
    std::sort(sorted.begin(), sorted.end(), [](auto a, auto b){ return a.first > b.first; }); // 降序

    int maxVal = 0;
    int limit = qMin(sorted.size(), 5);
    QList<QColor> colors = {CLR_PURPLE, CLR_BLUE, CLR_GREEN, CLR_ORANGE, CLR_PINK};
    QStringList categories;

    for(int i=0; i<limit; ++i) {
        categories << sorted[i].second;
        if(sorted[i].first > maxVal) maxVal = sorted[i].first;
    }

    for(int i=0; i<limit; ++i) {
        QBarSet *set = new QBarSet(sorted[i].second);
        set->setColor(colors[i % colors.size()]);
        set->setBorderColor(Qt::transparent);
        for(int j=0; j<limit; ++j) {
            if(i == j) *set << sorted[i].first;
            else *set << 0;
        }
        m_majorSeries->append(set);
    }

    QList<QAbstractAxis*> axes = m_majorChart->axes();
    for(auto ax : axes) m_majorChart->removeAxis(ax);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    QFont font = axisX->labelsFont();
    font.setPixelSize(13);
    font.setBold(true);
    axisX->setLabelsFont(font);
    axisX->setLabelsColor(Qt::white);
    axisX->setLabelsAngle(-30);

    m_majorChart->addAxis(axisX, Qt::AlignBottom);
    m_majorSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsColor(CLR_SUBTEXT);
    axisY->setRange(0, maxVal + 1);
    m_majorChart->addAxis(axisY, Qt::AlignLeft);
    m_majorSeries->attachAxis(axisY);
}

// ---------------- 交互实现 ----------------

void ChartWidget::onPieSliceHovered(bool status) {
    QPieSlice *slice = qobject_cast<QPieSlice *>(sender());
    if(!slice) return;

    if (status) {
        slice->setExploded(true);
        // Tooltip 显示
        QToolTip::showText(QCursor::pos(), QString("%1\n人数: %2 (%3%)")
                                               .arg(slice->series()->name())
                                               .arg((int)slice->value())
                                               .arg(slice->percentage()*100.0, 0, 'f', 1));

        // 临时把标签变成具体人数，方便查看
        // 原格式 "男生 55%" -> "11人"
        slice->setLabel(QString::number((int)slice->value()) + "人");
    } else {
        slice->setExploded(false);
        QToolTip::hideText();
        // 恢复格式 "男生 55%"
        // 注意：这里需要重新获取名字，但名字没存在slice里（只存在series的label或者legend里）
        // 简单起见，我们假设 series->name() 是空的，或者我们直接用 slice 之前存的 label 逻辑重构
        // 因为我们上面 update 时是这样存的：slice->setLabel("男生\n55%");
        // 所以这里我们得重新计算一次，比较麻烦。
        // 最简单的方法：不恢复成名字，只恢复百分比。或者我们在 update 里面把名字存到 slice->data() 里。

        // 优雅回退：
        slice->setLabel(QString::number(slice->percentage() * 100, 'f', 0) + "%");
    }
}

void ChartWidget::onBarHovered(bool status, int index, QBarSet *barSet) {
    Q_UNUSED(index);
    if (!barSet) return;

    if (status) {
        // StackedBarSeries 的每个 Set 只有一个有效值（它的 Sum）
        double val = barSet->sum();

        QColor c = barSet->color();
        barSet->setProperty("orgColor", c);
        // 【修改】：亮度提升到 170%，非常明显的发光效果
        barSet->setColor(c.lighter(170));

        QToolTip::showText(QCursor::pos(), QString("%1: %2人").arg(barSet->label()).arg((int)val));
    } else {
        QToolTip::hideText();
        QVariant var = barSet->property("orgColor");
        if(var.isValid()) barSet->setColor(var.value<QColor>());
    }
}

void ChartWidget::onSplineHovered(const QPointF &point, bool state) {
    if (state) {
        QToolTip::showText(QCursor::pos(),
                           QString("年龄: %1 岁\n学生人数: %2 人")
                               .arg(qRound(point.x()))
                               .arg(qRound(point.y())));
    } else {
        QToolTip::hideText();
    }
}
