#include "deletebuttondelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

// 构造函数
DeleteButtonDelegate::DeleteButtonDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

// 绘制方法实现
void DeleteButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    // 初始化样式选项
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 获取当前组件的样式
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    // 1. 绘制默认单元格背景（保持原始样式）
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

    // 2. 设置按钮区域（保留4像素边距）
    QRect rect = opt.rect.adjusted(4, 4, -4, -4);
    m_buttonRect = rect;  // 保存当前按钮区域用于点击检测

    // 3. 获取当前状态（悬停/选中）
    bool hover = (opt.state & QStyle::State_MouseOver);
    bool selected = (opt.state & QStyle::State_Selected);

    // 4. 设置按钮颜色
    QColor bgColor = hover ? QColor(240, 240, 240) : Qt::transparent; // 悬停时显示背景色
    QColor borderColor = selected ? QColor(160, 160, 160) : QColor(200, 200, 200); // 选中状态边框色更深

    // 5. 绘制按钮
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);  // 启用抗锯齿
    painter->setPen(borderColor);       // 设置边框颜色
    painter->setBrush(bgColor);         // 设置背景填充色
    painter->drawRoundedRect(rect, 4, 4); // 绘制圆角矩形（4像素圆角）

    // 6. 绘制文本：根据选择状态调整文字颜色
    QColor textColor = selected ? opt.palette.highlightedText().color() : opt.palette.text().color();
    painter->setPen(textColor);
    painter->drawText(rect, Qt::AlignCenter, "删除"); // 居中绘制"删除"文字
    painter->restore();
}

// 事件处理方法实现
bool DeleteButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index)
{
    // 检测鼠标释放事件（左键点击）
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        // 验证：左键点击 + 点击位置在按钮区域内
        if (me->button() == Qt::LeftButton && m_buttonRect.contains(me->pos())) {
            emit deleteClicked(index);  // 发出删除信号
            return true;  // 事件已处理
        }
    }
    // 其他事件交给父类处理
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

// 尺寸提示方法实现
QSize DeleteButtonDelegate::sizeHint(const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    // 设置固定宽度60px，保持原有行高
    return QSize(60, option.rect.height());
}
