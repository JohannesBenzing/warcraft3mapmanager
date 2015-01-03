#ifndef CustomWindow_H
#define CustomWindow_H

#include <QSqlTableModel>
#include <QtWidgets>

/**
  * Pixels around the border to mouse cursor change.
  **/
#define PIXELS_TO_ACT 5

namespace Ui
{
class CustomWindow;
}

/**
 * @brief The CustomWindow class features a styled border around the window and our own buttons for minimizing,maximizing,closing the window
 */
class CustomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWindow(QWidget *parent = 0);
    ~CustomWindow();

protected slots:
    /**
         * @brief moveWindow Method that moves the window to the new position when the window
         * is moved.
         * @param e The mouse event that sets the new position.
         */
    void moveWindow(QMouseEvent *e);

private:
    Ui::CustomWindow *ui;
    QRect beforeMaximizing;
    /**
         * @brief dragPosition Increment of the position movement.
         */
    QPoint dragPosition;
    /**
         * @brief moveWidget Specifies if the window is in move action.
         */
    bool moveWidget;
    /**
         * @brief inResizeZone Specifies if the mouse is in resize zone.
         */
    bool inResizeZone;
    /**
         * @brief allowToResize Specifies if the mouse is allowed to resize.
         */
    bool allowToResize;
    /**
         * @brief resizeVerSup Specifies if the resize is in the top of the window.
         */
    bool resizeVerSup;
    /**
         * @brief resizeHorEsq Specifies if the resize is in the left of the window.
         */
    bool resizeHorEsq;
    /**
         * @brief resizeDiagSupEsq Specifies if the resize is in the top left of the window.
         */
    bool resizeDiagSupEsq;
    /**
         * @brief resizeDiagSupDer Specifies if the resize is in the top right of the window.
         */
    bool resizeDiagSupDer;


    /**
         * @brief mouseMoveEvent Overloaded member that moves of resizes depending of the
         * configuration sotred at mousePressEvent().
         * @param e The mouse event.
         */
    void mouseMoveEvent(QMouseEvent *e);
    /**
         * @brief mousePressEvent Overloaded member that stores that changes the cursor and
         * configures the side that is changing.
         * @param e The mouse event.
         */
    void mousePressEvent(QMouseEvent *e);
    /**
         * @brief mouseReleaseEvent Overloaded member that removes the configuration set in mousePressEvent().
         * @param e The mouse event.
         */
    void mouseReleaseEvent(QMouseEvent *e);
    /**
         * @brief mouseDoubleClickEvent Overloadad member that maximize/restore the window if is
         * doubleclicked and the position of the mouse is not the top left of the window (menu zone).
         * @param e The mouse event.
         */
    void mouseDoubleClickEvent(QMouseEvent *e);
    /**
         * @brief paintEvent Overloaded method that allows to customize the styles of the window.
         */
    void paintEvent (QPaintEvent *);
    /**
         * @brief resizeWindow Method that calculates the resize and new position of the window an
         * does this actions.
         * @param e The mouse event to calculate the new position and size.
         */
    void resizeWindow(QMouseEvent *e);


    void resizeEvent(QResizeEvent* event);

    QSqlDatabase db;
    QSqlTableModel *model;
    int m_MinimumRating;
    int m_MinimumMaxPlayers;
    int m_MaxPlayers;
    QString m_sMapDir;
    QString m_sFilterIngameName;
    QString m_sFilterFileName;
    QString m_sFilterTags;

    /**
         * @brief Applies filters to SQLite dtabase
         */
    void updateFilters();

private slots:
    // buttons in titlebar
    void slotBtnMaxOrSmallWindow();
    void slotBtnMinimize();


    void slotSbMinimumRating(const int i);
    void slotSbMinimumMaxPlayers(int i);
    void slotSbMaxPlayers(int i);
    void slotLeFilterIngameName(const QString & text);
    void slotLeFilterFileName(const QString & text);
    void leFilterTagsSlot(const QString &s);
    void slotBtnLoad();
    void slotBtnResetFilters();
    void slotBtnSetMapDir();
    void slotBtnOpenInExplorer();
    void slotBtnMoveMap();
    void slotBtnCreateNewDB();


    void slotOnDataOfCellChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void slotSelectionChanged(const QItemSelection &, const QItemSelection &);
};

#endif // CustomWindow_H
