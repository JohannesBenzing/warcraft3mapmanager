#ifndef PREVENTEDITINGDELEGATE_H
#define PREVENTEDITINGDELEGATE_H

/**
     * @brief Prevents editing on assigned columns
     */
class PreventEditingDelegate : public QItemDelegate
{
public:
    explicit PreventEditingDelegate(QObject *parent = 0)
        : QItemDelegate(parent)
    {}
protected:
    /**
         * @brief Prevents creation of editor by returning Q_NULLPTR
         */
    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    { return Q_NULLPTR; }
};

#endif // PREVENTEDITINGDELEGATE_H
