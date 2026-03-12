#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QLabel>
#include <QGridLayout>

// --- Qt Charts 头文件 ---
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QStackedBarSeries> // 垂直堆叠（用于多彩柱状图）
#include <QtCharts/QHorizontalStackedBarSeries> // [新增] 水平堆叠（用于多彩条形图）



    class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();

private slots:
    void updateCharts(const QJsonArray &data);

    // --- 交互槽函数 ---
    void onPieSliceHovered(bool status);
    void onBarHovered(bool status, int index, QBarSet *barSet);
    void onSplineHovered(const QPointF &point, bool state);

private:
    void setupUi();
    void applyDashboardTheme(QChartView *view, const QString &title, bool showLegend = false);

    // 初始化图表
    void initGenderChart();
    void initClassChart();
    void initAgeChart();
    void initDeptChart();
    void initMajorChart();

    // 更新数据逻辑
    void updateKpiBoard(int total, int male, int female, double avgAge);
    void updateGenderChart(int male, int female);
    void updateClassChart(const QMap<QString, int> &classMap);
    void updateAgeChart(const QMap<int, int> &ageMap);
    void updateDeptChart(const QMap<QString, int> &deptMap);
    void updateMajorChart(const QMap<QString, int> &majorMap);

private:
    // --- 界面元素 ---
    QLabel *m_lblTotalCount;
    QLabel *m_lblGenderRatio;
    QLabel *m_lblAvgAge;

    QChartView *m_genderView;
    QChartView *m_classView;
    QChartView *m_deptView;
    QChartView *m_ageView;
    QChartView *m_majorView;

    // 数据系列
    QPieSeries *m_genderSeries;
    QPieSeries *m_classSeries;
    QSplineSeries *m_ageSeries;

    // [修改] 为了实现多彩，这两个图表都使用 Stacked 系列
    QHorizontalStackedBarSeries *m_deptSeries;
    QStackedBarSeries *m_majorSeries;

    // 图表对象
    QChart *m_genderChart;
    QChart *m_classChart;
    QChart *m_deptChart;
    QChart *m_ageChart;
    QChart *m_majorChart;
};

#endif // CHARTWIDGET_H
