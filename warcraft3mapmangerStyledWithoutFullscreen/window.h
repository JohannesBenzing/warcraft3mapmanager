//#ifndef WINDOW_H
//#define WINDOW_H

//#include <QObject>
//#include <QDialog>
//#include <QLineEdit>
//#include <QSpinBox>
//#include <QTableView>
//#include <QSqlDatabase>
//#include <QLabel>
//#include <QWidget>

//#include "QtWaitingSpinner.h"

//QT_BEGIN_NAMESPACE
//class QDialogButtonBox;
//class QPushButton;
//class QSqlTableModel;
//QT_END_NAMESPACE


////namespace Ui {
////class Widget;
////}
//namespace Ui
//{
//    class CustomWindow;
//}

//class Window : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit Window(QWidget *parent = 0);
//    ~Window();
//    void updateFilters();

//private slots:
//    void sbMinimumRatingSlot(const int i);
//    void sbMinimumMaxPlayersSlot(int i);
//    void sbMaxPlayersSlot(int i);
//    void leFilterIngameNameSlot(const QString & text);
//    void leFilterFileNameSlot(const QString & text);
//    void leFilterTagsSlot(const QString &s);
//    void btnLoadSlot();
//    void btnResetFiltersSlot();
//    void btnSetMapDirSlot();
//    void btnOpenInExplorerSlot();
//    void btnMoveMapSlot();
//    void btnCreateNewDBSlot();
//    void onDataOfCellChangedSlot(const QModelIndex& topLeft, const QModelIndex& bottomRight);
//    void selectionChangedSlot(const QItemSelection &, const QItemSelection &);

//private:
//    void resizeEvent(QResizeEvent* event);

////    Ui::Widget *ui;
//    Ui::CustomWindow *ui;

//    QSqlDatabase db;
//    QSqlTableModel *model;
//    int m_MinimumRating;
//    int m_MinimumMaxPlayers;
//    int m_MaxPlayers;
//    QString m_sMapDir;
//    QString m_sFilterIngameName;
//    QString m_sFilterFileName;
//    QString m_sFilterTags;
//    QtWaitingSpinner* m_spinner;
//};

//#endif
