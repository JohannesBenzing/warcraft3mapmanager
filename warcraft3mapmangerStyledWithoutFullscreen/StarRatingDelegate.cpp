#include "starratingdelegate.h"

#include <QtWidgets>

StarRatingDelegate::StarRatingDelegate(QObject *parent)
    : QItemDelegate(parent), star(QPixmap(":img/star.png"))
{
}

void StarRatingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *model = index.model();
    QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ?
                (option.state & QStyle::State_Active) ? QPalette::Normal : QPalette::Inactive : QPalette::Disabled;

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.color(cg, QPalette::Highlight));


    int rating = model->data(index, Qt::DisplayRole).toInt();

    int width = star.width();
    int height = star.height();
    int x = option.rect.x();
    int y = option.rect.y() + (option.rect.height() / 2) - (height / 2);
    for (int i = 0; i < rating; ++i) {
        painter->drawPixmap(x, y, star);
        x += width;
    }
    drawFocus(painter, option, option.rect.adjusted(0, 0, -1, -1)); // since we draw the grid ourselves

    QPen pen = painter->pen();
    painter->setPen(option.palette.color(QPalette::Mid));
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    painter->setPen(pen);
}

QSize StarRatingDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    return QSize(5 * star.width(), star.height()) + QSize(1, 1);
}

bool StarRatingDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int stars = qBound(0, int(0.7 + qreal(mouseEvent->pos().x()
                                              - option.rect.x()) / star.width()), 5);
        if(stars < 1){
            stars = 1;
        }
        model->setData(index, QVariant(stars));
        return false; //so that the selection can change
    }
    return true;
}
