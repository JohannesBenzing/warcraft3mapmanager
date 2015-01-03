#include "ViewCustomWindow.h"
#include "ui_CustomWindow.h"

#include <QtWidgets>

#include "window.h"

#include "DelegateStarRating.h"
#include "DelegatePreventEditing.h"
#include "ModelSqlite.h"

using namespace std;

CustomWindow::CustomWindow(ModelSqlite *modelSqlite, QWidget *parent)
    : QWidget(parent), ui(new Ui::CustomWindow)
{
    m_ModelSqlite = modelSqlite;
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);
    setWindowTitle(tr("Wc3 Map Manager"));
    setWindowIcon(QIcon(":/img/windowicon.ico"));
    setContentsMargins(0,0,0,0);
    setMinimumWidth(500);
    setMinimumHeight(550);
    m_beforeMaximizing = QRect(0,0, 500, 550);

    QDesktopWidget *desktop = QApplication::desktop();
    this->setGeometry(desktop->availableGeometry());


    ui->pbMin->setStyleSheet("QPushButton{border-image: url(:/img/btnMin.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMinG.png) 1;}");
    ui->pbSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindow.png) 1;}"
                                     "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    ui->pbMax->setStyleSheet("QPushButton{border-image: url(:/img/btnMaxG.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMaxG.png) 1;}");
    ui->pbFullscreen->setStyleSheet("QPushButton{border-image: url(:/img/btnFs.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnFsG.png) 1;}");
    ui->pbClose->setStyleSheet("QPushButton{border-image: url(:/img/btnClose.png) 1;}"
                               "QPushButton:hover{border-image: url(:/img/btnCloseG.png) 1;}");

    ui->btnSetMapDir->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}");
    ui->btnLoad->setEnabled(false);
    ui->btnLoad->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                               "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}"
                               "QPushButton:disabled{border-image: url(:/img/btnDisabled.png) 1;}");
    ui->btnCreateNewDB->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                      "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}");
    ui->btnResetFilters->setEnabled(false);
    ui->btnResetFilters->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                       "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}"
                                       "QPushButton:disabled{border-image: url(:/img/btnDisabled.png) 1;}");
    ui->btnOpenInExplorer->setEnabled(false);
    ui->btnOpenInExplorer->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                         "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}"
                                         "QPushButton:disabled{border-image: url(:/img/btnDisabled.png) 1;}");

    ui->btnMoveMap->setEnabled(false);
    ui->btnMoveMap->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                  "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}"
                                  "QPushButton:disabled{border-image: url(:/img/btnDisabled.png) 1;}");


    ui->titleBar->setStyleSheet("background-image: url(:/img/top.jpg)");
    ui->bottomBorder->setStyleSheet("background-image: url(:/img/top.jpg)");
    ui->leftBorder->setStyleSheet("background-image: url(:/img/left.jpg)");
    ui->rightBorder->setStyleSheet("background-image: url(:/img/right.jpg)");

    ui->lMapDirStr->setWordWrap(true);


    //setup QTableView
    ui->view->horizontalHeader()->setStretchLastSection(true);
    ui->view->resizeColumnsToContents();
    ui->view->setModel(m_ModelSqlite->getModel());
    ui->view->setSortingEnabled(true);
    ui->view->setItemDelegateForColumn(3, new StarRatingDelegate(ui->view));
    PreventEditingDelegate *deleg = new PreventEditingDelegate();
    ui->view->setItemDelegateForColumn(0, deleg);
    ui->view->setItemDelegateForColumn(1, deleg);
    ui->view->setItemDelegateForColumn(2, deleg);
    ui->view->setEditTriggers(QAbstractItemView::CurrentChanged); //single click creates editor to change cell
    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->setSelectionMode(QAbstractItemView::SingleSelection);


    //  fix the slider of the scrollbar of qtableview getting resized and jumping up a bit when dragging the slider down
    // probably because sqllite does not support Query size. (QSqlDriver::hasFeature(QuerySize))
    //    while(&model->canFetchMore()){
    //        &model->fetchMore();
    //    }
    while(m_ModelSqlite->getModel()->canFetchMore()){
        m_ModelSqlite->getModel()->fetchMore();
    }

    QString savedMapPath = m_ModelSqlite->createIfNotExistsSavedWarcraft3MapsPathAndReturnIt();
    if(!savedMapPath.isEmpty()){
        ui->lMapDirStr->setText(savedMapPath);
        m_sMapDir = savedMapPath;
        ui->btnLoad->setEnabled(true);
    }
    else{
        ui->lMapDirStr->setText("<none selected>");
    }


    m_moveWidget = false;
    m_inResizeZone = false;
    m_allowToResize = false;
    m_resizeVerSup = false;
    m_resizeHorEsq = false;
    m_resizeDiagSupEsq = false;
    m_resizeDiagSupDer = false;

    m_MinimumRating = 1;
    m_MinimumMaxPlayers = 1;
    m_MaxPlayers = 12;
    m_sFilterIngameName = "";
    m_sFilterFileName = "";

    setMouseTracking(true);
    ui->titleBar->setMouseTracking(true);
    ui->LTitle->setMouseTracking(true);
    ui->pbMin->setMouseTracking(true);
    ui->pbMax->setMouseTracking(true);
    ui->pbFullscreen->setMouseTracking(true);
    ui->pbClose->setMouseTracking(true);
    ui->pbSmallWindow->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->bottomBorder->setMouseTracking(true);
    ui->leftBorder->setMouseTracking(true);
    ui->rightBorder->setMouseTracking(true);

    ui->sbMinimumRating->setMinimum(1);
    ui->sbMinimumRating->setMaximum(5);


    ui->sbMinimumMaxPlayers->setMinimum(0);
    ui->sbMinimumMaxPlayers->setMaximum(12);


    ui->sbMaxPlayers->setMinimum(1);
    ui->sbMaxPlayers->setMaximum(12);
    ui->sbMaxPlayers->setValue(12);


    connect(ui->pbMin, SIGNAL(clicked()), this, SLOT(slotBtnMinimize()));
    connect(ui->pbMax, SIGNAL(clicked()), this, SLOT(slotBtnMaximize()));
    connect(ui->pbFullscreen, SIGNAL(clicked()), this, SLOT(slotBtnFullscreen()));
    connect(ui->pbSmallWindow, SIGNAL(clicked()), this, SLOT(slotBtnSmallWindow()));
    connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui->btnSetMapDir, SIGNAL(released()), this, SLOT(slotBtnSetMapDir()));
    connect(ui->btnLoad, SIGNAL(released()), this, SLOT(slotBtnLoad()));
    connect(ui->btnCreateNewDB, SIGNAL(released()), this, SLOT(slotBtnCreateNewDB()));
    connect(ui->btnResetFilters, SIGNAL(released()), this, SLOT(slotBtnResetFilters()));
    connect(ui->btnOpenInExplorer, SIGNAL(released()), this, SLOT(slotBtnOpenInExplorer()));
    connect(ui->btnMoveMap, SIGNAL(released()), this, SLOT(slotBtnMoveMap()));

    connect(ui->sbMinimumRating, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMinimumRating(const int)));
    connect(ui->sbMinimumMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMinimumMaxPlayers(int)));
    connect(ui->sbMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMaxPlayers(int)));

    connect(ui->leFilterIngameName,SIGNAL(textChanged(const QString &)),this,SLOT(slotLeFilterIngameName(const QString &)));
    connect(ui->leFilterFileName,SIGNAL(textChanged(const QString &)),this,SLOT(slotLeFilterFileName(const QString &)));
    connect(ui->leFilterTags,SIGNAL(textChanged(const QString &)),this,SLOT(leFilterTagsSlot(const QString &)));

    connect(m_ModelSqlite->getModel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotOnDataOfCellChanged(const QModelIndex&, const QModelIndex&)));
    connect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &))
            );
    ui->view->resizeColumnsToContents();
}

CustomWindow::~CustomWindow()
{
    delete ui;
}

void CustomWindow::mouseMoveEvent(QMouseEvent *e)
{
    int xMouse = e->pos().x();
    int yMouse = e->pos().y();
    int wWidth = geometry().width();
    int wHeight = geometry().height();

    if (m_moveWidget)
    {
        m_inResizeZone = false;
        moveWindow(e);
    }
    else if (m_allowToResize)
        resizeWindow(e);
    else if (xMouse >= wWidth - PIXELS_TO_ACT or m_allowToResize)
    {
        m_inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }
    else if (xMouse <= PIXELS_TO_ACT or m_allowToResize)
    {
        m_inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }
    else if ((yMouse >= wHeight - PIXELS_TO_ACT) or m_allowToResize)
    {
        m_inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }
    else if (yMouse <= PIXELS_TO_ACT or m_allowToResize)
    {
        m_inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }
    else
    {
        m_inResizeZone = false;
        setCursor(Qt::ArrowCursor);
    }
    if(isFullScreen()){
        setCursor(Qt::ArrowCursor);
        e->accept();
        return;
    }
    QDesktopWidget *desktop = QApplication::desktop();
    if(this->geometry() == desktop->availableGeometry()){ //window is in fullscreen-mode
        setCursor(Qt::ArrowCursor);
        e->accept();
        return;
    }

    e->accept();
}

void CustomWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (m_inResizeZone)
        {
            if (e->pos().y() <= PIXELS_TO_ACT)
            {
                if (e->pos().x() <= PIXELS_TO_ACT)
                    m_resizeDiagSupEsq = true;
                else if (e->pos().x() >= geometry().width() - PIXELS_TO_ACT)
                    m_resizeDiagSupDer = true;
                else
                    m_resizeVerSup = true;
            }
            else if (e->pos().x() <= PIXELS_TO_ACT)
                m_resizeHorEsq = true;


            m_allowToResize = true;
        }
        else if (e->pos().x() >= PIXELS_TO_ACT and e->pos().x() < ui->titleBar->geometry().width()
                 and e->pos().y() >= PIXELS_TO_ACT and e->pos().y() < ui->titleBar->geometry().height())
        {
            if(isFullScreen()){
                e->accept();
                return;
            }
            QDesktopWidget *desktop = QApplication::desktop();
            if(this->geometry() == desktop->availableGeometry()){ //window is in fullscreen-mode
                e->accept();
                return;
            }
            m_moveWidget = true;
            m_dragPosition = e->globalPos() - frameGeometry().topLeft();
        }
    }

    e->accept();
}

void CustomWindow::mouseReleaseEvent(QMouseEvent *e)
{
    m_moveWidget = false;
    m_allowToResize = false;
    m_resizeVerSup = false;
    m_resizeHorEsq = false;
    m_resizeDiagSupEsq = false;
    m_resizeDiagSupDer = false;

    e->accept();
}

void CustomWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (    e->pos().x() < ui->titleBar->geometry().width()
            && e->pos().y() < ui->titleBar->geometry().height()
            ){

        QDesktopWidget *desktop = QApplication::desktop();
        if(this->geometry() == desktop->availableGeometry()){
            slotBtnSmallWindow();

        }
        else{ //window is small or in fullscreenmode
            slotBtnMaximize();
        }
    }
    e->accept();
}

void CustomWindow::paintEvent (QPaintEvent *)
{
    QStyleOption opt;
    opt.init (this);
    QPainter p(this);
    style()->drawPrimitive (QStyle::PE_Widget, &opt, &p, this);
}

void CustomWindow::moveWindow(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
    {
        move(e->globalPos() - m_dragPosition);
        e->accept();
    }
}

void CustomWindow::resizeWindow(QMouseEvent *e)
{
    if (m_allowToResize)
    {
        int xMouse = e->pos().x();
        int yMouse = e->pos().y();
        int wWidth = geometry().width();
        int wHeight = geometry().height();

        if (cursor().shape() == Qt::SizeVerCursor)
        {
            if (m_resizeVerSup)
            {
                int newY = geometry().y() + yMouse;
                int newHeight = wHeight - yMouse;

                if (newHeight > minimumSizeHint().height())
                {
                    resize(wWidth, newHeight);
                    move(geometry().x(), newY);
                }
            }
            else
                resize(wWidth, yMouse+1);
        }
        else if (cursor().shape() == Qt::SizeHorCursor)
        {
            if (m_resizeHorEsq)
            {
                int newX = geometry().x() + xMouse;
                int newWidth = wWidth - xMouse;

                if (newWidth > minimumSizeHint().width())
                {
                    resize(newWidth, wHeight);
                    move(newX, geometry().y());
                }
            }
            else
                resize(xMouse, wHeight);
        }
        else if (cursor().shape() == Qt::SizeBDiagCursor)
        {
            int newX = 0;
            int newWidth = 0;
            int newY = 0;
            int newHeight = 0;

            if (m_resizeDiagSupDer)
            {
                newX = geometry().x();
                newWidth = xMouse;
                newY = geometry().y() + yMouse;
                newHeight = wHeight - yMouse;
            }
            else
            {
                newX = geometry().x() + xMouse;
                newWidth = wWidth - xMouse;
                newY = geometry().y();
                newHeight = yMouse;
            }

            if (newWidth >= minimumSizeHint().width() and newHeight >= minimumSizeHint().height())
            {
                resize(newWidth, newHeight);
                move(newX, newY);
            }
            else if (newWidth >= minimumSizeHint().width())
            {
                resize(newWidth, wHeight);
                move(newX, geometry().y());
            }
            else if (newHeight >= minimumSizeHint().height())
            {
                resize(wWidth, newHeight);
                move(geometry().x(), newY);
            }
        }
        else if (cursor().shape() == Qt::SizeFDiagCursor)
        {
            if (m_resizeDiagSupEsq)
            {
                int newX = geometry().x() + xMouse;
                int newWidth = wWidth - xMouse;
                int newY = geometry().y() + yMouse;
                int newHeight = wHeight - yMouse;

                if (newWidth >= minimumSizeHint().width() and newHeight >= minimumSizeHint().height())
                {
                    resize(newWidth, newHeight);
                    move(newX, newY);
                }
                else if (newWidth >= minimumSizeHint().width())
                {
                    resize(newWidth, wHeight);
                    move(newX, geometry().y());
                }
                else if (newHeight >= minimumSizeHint().height())
                {
                    resize(wWidth, newHeight);
                    move(geometry().x(), newY);
                }
            }
            else
                resize(xMouse+1, yMouse+1);
        }

        e->accept();
    }
}

void CustomWindow::slotBtnMinimize()
{
    if (isMinimized())
    {
        setWindowState(windowState() & ~Qt::WindowMinimized);
    }
    else
    {
        setWindowState(windowState() | Qt::WindowMinimized);
    }
}

void CustomWindow::slotBtnSmallWindow(){
    ui->pbSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindowG.png) 1;}"
                                     "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    ui->pbMax->setStyleSheet("QPushButton{border-image: url(:/img/btnMax.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMaxG.png) 1;}");
    ui->pbFullscreen->setStyleSheet("QPushButton{border-image: url(:/img/btnFs.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnFsG.png) 1;}");

    if(this->geometry() != m_beforeMaximizing){
        this->showNormal();
        this->setGeometry(m_beforeMaximizing);
    }
}

void CustomWindow::slotBtnMaximize()
{
    QDesktopWidget *desktop = QApplication::desktop();
    if(! isFullScreen()){
        if(! (this->geometry() == desktop->availableGeometry())){
            m_beforeMaximizing = this->geometry();
        }
    }

    this->showMaximized();
    this->setGeometry(desktop->availableGeometry());

    ui->pbSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindow.png) 1;}"
                                     "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    ui->pbMax->setStyleSheet("QPushButton{border-image: url(:/img/btnMaxG.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMaxG.png) 1;}");
    ui->pbFullscreen->setStyleSheet("QPushButton{border-image: url(:/img/btnFs.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnFsG.png) 1;}");
}

void CustomWindow::slotBtnFullscreen()
{
    QDesktopWidget *desktop = QApplication::desktop();
    if(! (this->geometry() == desktop->availableGeometry())){
        m_beforeMaximizing = this->geometry();
    }

    this->showFullScreen();

    ui->pbSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindow.png) 1;}"
                                     "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    ui->pbMax->setStyleSheet("QPushButton{border-image: url(:/img/btnMax.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMaxG.png) 1;}");
    ui->pbFullscreen->setStyleSheet("QPushButton{border-image: url(:/img/btnFsG.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnFsG.png) 1;}");
}

void CustomWindow::resizeEvent(QResizeEvent* event){
    ui->view->setColumnWidth(2,62);
}

void CustomWindow::slotOnDataOfCellChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);
}
void CustomWindow::slotSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    ui->btnOpenInExplorer->setEnabled(true);
    ui->btnMoveMap->setEnabled(true);
    disconnect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &))
               );
}
void CustomWindow::slotSbMinimumRating(const int i){
    m_MinimumRating = i;
    updateFilters();
}
void CustomWindow::slotSbMinimumMaxPlayers(int i){
    if(i > m_MaxPlayers){
        m_MaxPlayers = i;
        ui->sbMaxPlayers->setValue(i);
    }
    m_MinimumMaxPlayers = i;
    updateFilters();
}
void CustomWindow::slotSbMaxPlayers(int i){
    if(i < m_MinimumMaxPlayers){
        m_MinimumMaxPlayers = i;
        ui->sbMinimumMaxPlayers->setValue(i);
    }
    m_MaxPlayers = i;
    updateFilters();
}
void CustomWindow::slotLeFilterIngameName(const QString &s){
    m_sFilterIngameName = s;
    m_sFilterIngameName.replace(QString("'"), QString("''"));
    updateFilters();
}
void CustomWindow::slotLeFilterFileName(const QString &s){
    m_sFilterFileName = s;
    m_sFilterFileName.replace(QString("'"), QString("''"));
    updateFilters();
}
void CustomWindow::leFilterTagsSlot(const QString &s){
    m_sFilterTags = s;
    m_sFilterTags.replace(QString("'"), QString("''"));
    updateFilters();
}
void CustomWindow::updateFilters(){
    ui->btnResetFilters->setEnabled(true);
    QStringList sl = m_sFilterTags.split(" ", QString::SkipEmptyParts);
    QString sqlTags;
    for (QStringList::iterator it = sl.begin();it != sl.end(); ++it) {
        sqlTags.append(QString(" and tags LIKE '%%1%'").arg(*it));
    }
    m_ModelSqlite->setFilters(QString("rating >= '%1' and maximumPlayers >= '%2' and maximumPlayers <= '%3' and ingamename LIKE '%%4%' and filename LIKE '%%5%'%6").arg(m_MinimumRating).arg(m_MinimumMaxPlayers).arg(m_MaxPlayers).arg(m_sFilterIngameName).arg(m_sFilterFileName).arg(sqlTags));
}
void CustomWindow::slotBtnSetMapDir()
{
    QString s = QFileDialog::getExistingDirectory(this, tr("Open Warcraft 3 Maps folder (default: C:/Program Files/Warcraft III/Maps )"),
                                                  "/home",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
    if(!s.isEmpty()){ //Directory-choose-dialog was not aborted
        m_sMapDir = s;
        ui->lMapDirStr->setText(s);
        ui->btnLoad->setEnabled(true);
        QFile file("savedWarcraft3MapsPath");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << s;
        file.close();
    }
}
void CustomWindow::slotBtnResetFilters()
{
    m_MinimumRating = 1;
    m_MinimumMaxPlayers = 1;
    m_MaxPlayers = 12;
    m_sFilterIngameName = "";
    m_sFilterFileName = "";
    ui->sbMinimumRating->setValue(0);
    ui->sbMinimumMaxPlayers->setValue(0);
    ui->sbMaxPlayers->setValue(12);
    ui->leFilterIngameName->setText("");
    ui->leFilterFileName->setText("");
    ui->leFilterTags->setText("");

    updateFilters();

    ui->btnResetFilters->setEnabled(false);
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);

    //  fix the slider of the scrollbar of qtableview getting resized and jumping up a bit when dragging the slider down
    // probably because sqllite does not support Query size. (QSqlDriver::hasFeature(QuerySize))
    //    while(model->canFetchMore()){
    //        model->fetchMore();
    //    }
    while(m_ModelSqlite->getModel()->canFetchMore()){
        m_ModelSqlite->getModel()->fetchMore();
    }
}
void CustomWindow::slotBtnCreateNewDB(){
    m_ModelSqlite->createNewDB();

    ui->view->setColumnWidth(2,62); //"max player" column fixed width because there will only be values between 0 and 12 inside

    ui->btnOpenInExplorer->setEnabled(false);
    ui->btnMoveMap->setEnabled(false);
    connect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &))
               );
}
void CustomWindow::slotBtnOpenInExplorer()
{
    QString selectedFileName = m_ModelSqlite->getModel()->index(ui->view->selectionModel()->currentIndex().row() , 1).data().toString();
    QDir *dir = new QDir(m_sMapDir);
    QDirIterator iterator(dir->absolutePath(), QDirIterator::Subdirectories);
    while (iterator.hasNext()){
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.fileName();
            if (filename == selectedFileName){
                QString fullpath = iterator.filePath();
                //works only on windows
                QStringList args;
                args << "/select," << QDir::toNativeSeparators(fullpath);
                QProcess::startDetached("explorer", args);
                return;
            }
        }
    }
    QMessageBox msgBox;
    msgBox.setText("Map could not be found. Maybe you changed the map-path or you deleted the map.");
    msgBox.exec();
}
void CustomWindow::slotBtnMoveMap()
{
    QString selectedFileName = m_ModelSqlite->getModel()->index(ui->view->selectionModel()->currentIndex().row() , 1).data().toString();
    QDir *dir = new QDir(m_sMapDir);
    QDirIterator iterator(dir->absolutePath(), QDirIterator::Subdirectories);
    boolean bFileFound = false;
    while (iterator.hasNext()){
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.fileName();
            if (filename == selectedFileName){
                QString fullpath = iterator.filePath();
                if(fullpath == QString(m_sMapDir).append("/").append(filename)){
                    bFileFound = true;
                    break;
                }
                else if(QFile::rename(fullpath , QString(m_sMapDir).append("/").append(filename))){
                    bFileFound = true;
                }
                break;
            }
        }
    }
    if(!bFileFound){
        QMessageBox msgBox;
        msgBox.setText("File could not be moved. Maybe it is write-protected.");
        msgBox.exec();
        return;
    }

    QString pathBackup = QString(m_sMapDir).append("/0Backup/");
    QDir dirBackup(pathBackup);
    if(! dirBackup.exists()){
        if(! QDir().mkdir(pathBackup)){
            QMessageBox msgBox;
            msgBox.setText("Backup-folder '0Backup' could not be created. Maybe your game-folder is write-protected");
            msgBox.exec();
            return;
        }
    }
    QDirIterator iterator2(dir->absolutePath(), QDirIterator::NoIteratorFlags);
    while (iterator2.hasNext()){
        iterator2.next();
        if (!iterator2.fileInfo().isDir()){
            QString filename = iterator2.fileName();
            if (filename != selectedFileName){
                QString fullpath = iterator2.filePath();
                QFile::rename(fullpath, QString(pathBackup).append(filename));
            }
        }
    }
}
void CustomWindow::slotBtnLoad()
{
    ui->view->hide();
    QLabel* label = new QLabel(ui->centralRight);
    label->setMask((new QPixmap(":img/loading.gif"))->mask());
    QMovie *movie = new QMovie(":img/loading.gif");
    label->setMovie(movie);
    movie->start();
    label->resize(ui->view->size());
    label->show();


    m_ModelSqlite->loadMapDataRecursivelyFromDir(m_sMapDir);


    label->hide();
    ui->view->show();
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,64);
}

