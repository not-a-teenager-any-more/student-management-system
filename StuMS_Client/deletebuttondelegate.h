#ifndef DELETEBUTTONDELEGATE_H
#define DELETEBUTTONDELEGATE_H

#include <QStyledItemDelegate>

class DeleteButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DeleteButtonDelegate(QObject *parent = nullptr);

    // 重写绘制方法：负责在单元格内绘制删除按钮
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 重写编辑器事件方法：处理鼠标点击事件
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

    // 重写尺寸提示方法：定义按钮的推荐尺寸
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    // 删除按钮点击信号：当按钮被点击时发射，传递对应的模型索引
    void deleteClicked(const QModelIndex &index);

private:
    // 保存按钮绘制区域，用于点击检测
    mutable QRect m_buttonRect;
};

#endif // DELETEBUTTONDELEGATE_H
