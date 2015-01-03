#ifndef PREVENTEDITINGDELEGATE_H
#define PREVENTEDITINGDELEGATE_H

/**
     * @brief Prevents editing on some columns
     */
class PreventEditingDelegate : public QItemDelegate
{
public:
    explicit PreventEditingDelegate(QObject *parent = 0)
        : QItemDelegate(parent)
    {}
protected:
    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    { return Q_NULLPTR; }
};

#endif // PREVENTEDITINGDELEGATE_H
