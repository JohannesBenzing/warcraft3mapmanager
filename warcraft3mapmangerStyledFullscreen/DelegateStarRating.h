#ifndef STARRATINGDELEGATE_H
#define STARRATINGDELEGATE_H

#include <QItemDelegate>

/**
     * @brief Assigned columns have a 5 stars rating. Click inside the cells to change the amount of stars
     */
class StarRatingDelegate : public QItemDelegate
{
public:
    StarRatingDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);

protected:
    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    { return Q_NULLPTR; }

    private:
        QPixmap m_star;
};

#endif
