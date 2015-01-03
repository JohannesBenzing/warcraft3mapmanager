#ifndef PREVENTEDITINGDELEGATE_H
#define PREVENTEDITINGDELEGATE_H

#include <QWidget>
//#include <QStyleOptionViewItem>
//#include <QItemDelegate>

class preventeditingdelegate : public QItemDelegate
{
public:
    explicit preventeditingdelegate(QObject *parent = 0)
        : QItemDelegate(parent)
    {}
protected:
    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    { return Q_NULLPTR; }
};

#endif // PREVENTEDITINGDELEGATE_H
