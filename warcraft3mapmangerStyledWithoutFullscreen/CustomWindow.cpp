#include "CustomWindow.h"
#include "ui_CustomWindow.h"

#include <QtWidgets>
#include <QtSql>

#include <fstream>
#include <sstream>

#include "window.h"

#include "StarRatingDelegate.h"
#include "PreventEditingDelegate.h"

using namespace std;

CustomWindow::CustomWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::CustomWindow)
{    
    ui->setupUi(this);

    ui->pbMin->setStyleSheet("QPushButton{border-image: url(:/img/btnMin.png) 1;}"
                             "QPushButton:hover{border-image: url(:/img/btnMinG.png) 1;}");
    ui->pbMaxOrSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindow.png) 1;}"
                                          "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    ui->pbClose->setStyleSheet("QPushButton{border-image: url(:/img/btnClose.png) 1;}"
                               "QPushButton:hover{border-image: url(:/img/btnCloseG.png) 1;}");

    ui->btnSetMapDir->setStyleSheet("QPushButton{border-image: url(:/img/btnHell.png) 1;}"
                                    "QPushButton:hover{border-image: url(:/img/btnG.png) 1;}");
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

    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    setMouseTracking(true);
    ui->titleBar->setMouseTracking(true);
    ui->LTitle->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    ui->bottomBorder->setMouseTracking(true);
    ui->leftBorder->setMouseTracking(true);
    ui->rightBorder->setMouseTracking(true);

    connect(ui->pbMin, SIGNAL(clicked()), this, SLOT(slotBtnMinimize()));
    connect(ui->pbMaxOrSmallWindow, SIGNAL(clicked()), this, SLOT(slotBtnMaxOrSmallWindow()));
    connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));

    //Per poder rebre les dades del ratol� sense haver de clicar cap bot�
    moveWidget = false;
    inResizeZone = false;
    allowToResize = false;
    resizeVerSup = false;
    resizeHorEsq = false;
    resizeDiagSupEsq = false;
    resizeDiagSupDer = false;

    m_MinimumRating = 1;
    m_MinimumMaxPlayers = 1;
    m_MaxPlayers = 12;
    m_sFilterIngameName = "";
    m_sFilterFileName = "";

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Warcraft3Maps.db");
    if (! db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Database error! Please check if 'Warcraft3Maps.db.db' is write-protected");
        msgBox.addButton("Exit program", QMessageBox::ActionRole);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    else{
        QSqlQuery query(db);
        if (! query.exec("CREATE TABLE IF NOT EXISTS maps (ingamename VARCHAR(45) NULL,filename VARCHAR(99) NULL,maximumPlayers INT NULL,rating INT NULL,tags VARCHAR(99) NULL);")){
            QMessageBox msgBox;
            //            qDebug() << query.lastError();
            msgBox.setText("Database error! Please check if 'Warcraft3Maps.db.db' is write-protected");
            msgBox.addButton("Exit program", QMessageBox::ActionRole);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    }
    model = new QSqlTableModel(this);
    model->setTable("maps");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "Ingame-name");
    model->setHeaderData(1, Qt::Horizontal, "File-name");
    model->setHeaderData(2, Qt::Horizontal, "Max.\n players");
    model->setHeaderData(3, Qt::Horizontal, "Rating");
    model->setHeaderData(4, Qt::Horizontal, "Tags");

    ui->view->setModel(model);
    ui->view->setSortingEnabled(true);
    ui->view->setItemDelegateForColumn(3, new StarRatingDelegate(ui->view));
    PreventEditingDelegate *deleg = new PreventEditingDelegate();
    ui->view->setItemDelegateForColumn(0, deleg);
    ui->view->setItemDelegateForColumn(1, deleg);
    ui->view->setItemDelegateForColumn(2, deleg);
    ui->view->setEditTriggers(QAbstractItemView::CurrentChanged); //single click creates editor to change cell
    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &))
            );

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotOnDataOfCellChanged(const QModelIndex&, const QModelIndex&)));

    ui->btnLoad->setEnabled(false);
    connect(ui->btnLoad, SIGNAL(released()), this, SLOT(slotBtnLoad()));

    QFile pathfile("savedWarcraft3MapsPath");
    if ( pathfile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&pathfile);
        QString line = in.readLine();
        if(!line.isEmpty()){
            ui->lMapDirStr->setText(line);
            m_sMapDir = line;
            ui->btnLoad->setEnabled(true);
        }
        else{
            ui->lMapDirStr->setText("<none selected>");
        }
    }
    else{
        ui->lMapDirStr->setText("<none selected>");
    }
    pathfile.close();

    ui->lMapDirStr->setWordWrap(true);

    connect(ui->btnSetMapDir, SIGNAL(released()), this, SLOT(slotBtnSetMapDir()));

    connect(ui->btnResetFilters, SIGNAL(released()), this, SLOT(slotBtnResetFilters()));

    connect(ui->btnCreateNewDB, SIGNAL(released()), this, SLOT(slotBtnCreateNewDB()));

    ui->sbMinimumRating->setMinimum(1);
    ui->sbMinimumRating->setMaximum(5);
    connect(ui->sbMinimumRating, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMinimumRating(const int)));

    ui->sbMinimumMaxPlayers->setMinimum(0);
    ui->sbMinimumMaxPlayers->setMaximum(12);
    connect(ui->sbMinimumMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMinimumMaxPlayers(int)));

    ui->sbMaxPlayers->setMinimum(1);
    ui->sbMaxPlayers->setMaximum(12);
    ui->sbMaxPlayers->setValue(12);
    connect(ui->sbMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(slotSbMaxPlayers(int)));

    connect(ui->leFilterIngameName,SIGNAL(textChanged(const QString &)),this,SLOT(slotLeFilterIngameName(const QString &)));

    connect(ui->leFilterFileName,SIGNAL(textChanged(const QString &)),this,SLOT(slotLeFilterFileName(const QString &)));

    connect(ui->leFilterTags,SIGNAL(textChanged(const QString &)),this,SLOT(leFilterTagsSlot(const QString &)));


    connect(ui->btnOpenInExplorer, SIGNAL(released()), this, SLOT(slotBtnOpenInExplorer()));

    connect(ui->btnMoveMap, SIGNAL(released()), this, SLOT(slotBtnMoveMap()));

    setContentsMargins(0,0,0,0);
    setMinimumWidth(500);
    setMinimumHeight(550);
    beforeMaximizing = QRect(0,0, 500, 550);

    ui->view->resizeColumnsToContents();

    setWindowTitle(tr("Wc3 Map Manager"));
    setWindowIcon(QIcon(":/img/windowicon.ico"));

    this->showMaximized();

    ui->view->horizontalHeader()->setStretchLastSection(true);

    //  fix the slider of the scrollbar of qtableview getting resized and jumping up a bit when dragging the slider down
    // probably because sqllite does not support Query size. (QSqlDriver::hasFeature(QuerySize))
    while(model->canFetchMore()){
        model->fetchMore();
    }
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

    if (moveWidget)
    {
        inResizeZone = false;
        moveWindow(e);
    }
    else if (allowToResize)
        resizeWindow(e);
    //Cursor part dreta
    else if (xMouse >= wWidth - PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }
    //Cursor part esquerra
    else if (xMouse <= PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;

        if (yMouse >= wHeight - PIXELS_TO_ACT)
            setCursor(Qt::SizeBDiagCursor);
        else if (yMouse <= PIXELS_TO_ACT)
            setCursor(Qt::SizeFDiagCursor);
        else
            setCursor(Qt::SizeHorCursor);

        resizeWindow(e);
    }
    //Cursor part inferior
    else if ((yMouse >= wHeight - PIXELS_TO_ACT) or allowToResize)
    {
        inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }
    //Cursor part superior
    else if (yMouse <= PIXELS_TO_ACT or allowToResize)
    {
        inResizeZone = true;
        setCursor(Qt::SizeVerCursor);

        resizeWindow(e);
    }
    else
    {
        inResizeZone = false;
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
        if (inResizeZone)
        {
            if (e->pos().y() <= PIXELS_TO_ACT)
            {
                if (e->pos().x() <= PIXELS_TO_ACT)
                    resizeDiagSupEsq = true;
                else if (e->pos().x() >= geometry().width() - PIXELS_TO_ACT)
                    resizeDiagSupDer = true;
                else
                    resizeVerSup = true;
            }
            else if (e->pos().x() <= PIXELS_TO_ACT)
                resizeHorEsq = true;


            allowToResize = true;
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
            moveWidget = true;
            dragPosition = e->globalPos() - frameGeometry().topLeft();
        }
    }

    e->accept();
}

void CustomWindow::mouseReleaseEvent(QMouseEvent *e)
{
    moveWidget = false;
    allowToResize = false;
    resizeVerSup = false;
    resizeHorEsq = false;
    resizeDiagSupEsq = false;
    resizeDiagSupDer = false;

    e->accept();
}

void CustomWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (    e->pos().x() < ui->titleBar->geometry().width()
            && e->pos().y() < ui->titleBar->geometry().height()
            ){
        slotBtnMaxOrSmallWindow();
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
        move(e->globalPos() - dragPosition);
        e->accept();
    }
}

void CustomWindow::resizeWindow(QMouseEvent *e)
{
    if (allowToResize)
    {
        int xMouse = e->pos().x();
        int yMouse = e->pos().y();
        int wWidth = geometry().width();
        int wHeight = geometry().height();

        if (cursor().shape() == Qt::SizeVerCursor)
        {
            if (resizeVerSup)
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
            if (resizeHorEsq)
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

            if (resizeDiagSupDer)
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
            if (resizeDiagSupEsq)
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

void CustomWindow::slotBtnMaxOrSmallWindow(){
    QDesktopWidget *desktop = QApplication::desktop();
    if(this->geometry() == desktop->availableGeometry()){
        this->showNormal();
        ui->pbMaxOrSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnMax.png) 1;}"
                                              "QPushButton:hover{border-image: url(:/img/btnMaxG.png) 1;}");
    }
    else{
        this->showMaximized();
        ui->pbMaxOrSmallWindow->setStyleSheet("QPushButton{border-image: url(:/img/btnSmallWindow.png) 1;}"
                                              "QPushButton:hover{border-image: url(:/img/btnSmallWindowG.png) 1;}");
    }
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
    model->setFilter(QString("rating >= '%1' and maximumPlayers >= '%2' and maximumPlayers <= '%3' and ingamename LIKE '%%4%' and filename LIKE '%%5%'%6").arg(m_MinimumRating).arg(m_MinimumMaxPlayers).arg(m_MaxPlayers).arg(m_sFilterIngameName).arg(m_sFilterFileName).arg(sqlTags));
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
    while(model->canFetchMore()){
        model->fetchMore();
    }
}
void CustomWindow::slotBtnCreateNewDB(){
    if (! db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Database error! Please check if 'Warcraft3Maps.db' is write-protected");
        msgBox.addButton("Exit program", QMessageBox::ActionRole);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    QSqlQuery query(db);
    if(query.exec(QString("DROP TABLE IF EXISTS maps;"))){
        query.exec(QString("CREATE TABLE maps (ingamename VARCHAR(45) NULL,filename VARCHAR(99) NULL,maximumPlayers INT NULL,rating INT NULL,tags VARCHAR(99) NULL);"));
    }
    model->setTable("maps");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "Ingame-name");
    model->setHeaderData(1, Qt::Horizontal, "File-name");
    model->setHeaderData(2, Qt::Horizontal, "Max.\n players");
    model->setHeaderData(3, Qt::Horizontal, "Rating");
    model->setHeaderData(4, Qt::Horizontal, "Tags");
    ui->view->setColumnWidth(2,62);
}
void CustomWindow::slotBtnOpenInExplorer()
{
    QString selectedFileName = model->index(ui->view->selectionModel()->currentIndex().row() , 1).data().toString();
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
    QString selectedFileName = model->index(ui->view->selectionModel()->currentIndex().row() , 1).data().toString();
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

    QSqlQuery query(db);

    //  more performance but might corrupte database on crash
    //  http://stackoverflow.com/questions/1711631/how-do-i-improve-the-performance-of-sqlite
    //  much faster!!
    query.exec("PRAGMA synchronous = OFF");
    query.exec("PRAGMA journal_mode = MEMORY");
    query.exec("BEGIN TRANSACTION");

    QDir *dir = new QDir(m_sMapDir);
    QDirIterator iterator(dir->absolutePath(), QDirIterator::Subdirectories);
    boolean bSkipOpenError = false;
    boolean bSkipOpenMaxPlayerError = false;

    int counter = 0;

    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.fileName();
            if (filename.endsWith(".w3x") || filename.endsWith(".w3m")){

                qApp->processEvents(QEventLoop::ExcludeUserInputEvents); //allows animated loading

                query.exec(QString("SELECT * FROM maps WHERE filename = '%1';").arg(filename.replace(QString("'"), QString("''"))));
                if(query.next()){
                    continue; //map is already in DB
                }
                QString fullpath = iterator.filePath();
                QByteArray ba = fullpath.toLocal8Bit();
                const char *pMapname = ba.data();

                unsigned char x;
                ifstream input1(pMapname, std::ios::binary);
                if (!input1)
                {
                    if(bSkipOpenMaxPlayerError){
                        continue;
                    }
                    QMessageBox msgBox;
                    msgBox.setText(QString("Unable to open: %1                                          %2").arg(filename).arg(fullpath));
                    QPushButton *btnSkipOpenError = msgBox.addButton("Ignore all following errors and continue", QMessageBox::ActionRole);
                    QPushButton *btnContinue = msgBox.addButton("Ignore and continue", QMessageBox::ActionRole);
                    QPushButton *btnAbort = msgBox.addButton("Exit program", QMessageBox::ActionRole);
                    msgBox.exec();
                    if (msgBox.clickedButton() == btnSkipOpenError) {
                        bSkipOpenMaxPlayerError = true;
                        continue;
                    } else if (msgBox.clickedButton() == btnContinue) {
                        continue;
                    } else if (msgBox.clickedButton() == btnAbort) {
                        exit(EXIT_FAILURE);
                    }
                }
                input1 >> noskipws;
                unsigned int i = 0;
                unsigned int maxNumPlayers = 0;
                while (input1 >> x)
                {
                    i++;
                    if (i>70)
                    {
                        break;
                    }
                    //    cout << std::hex << std::setw(2) << std::setfill('0') << (int)x << " ";
                    if ((int)x != 0)
                    {
                        maxNumPlayers = x;
                    }
                }
                input1.close();
                //                printf("Maximum players: %d \n", maxNumPlayers);

                //some maps even ingame only show 0 as maximum players(for example: Icegliders 1.2.w3x)
                if (maxNumPlayers < 1 || maxNumPlayers > 12){
                    maxNumPlayers = 0;
                }

                QFile file(pMapname);
                if (!file.open(QIODevice::ReadOnly))
                {
                    if(bSkipOpenError){
                        continue;
                    }
                    QMessageBox msgBox;
                    msgBox.setText(QString("Unable to open: %1                                          %2").arg(filename).arg(fullpath));
                    QPushButton *btnSkipOpenError = msgBox.addButton("Ignore all following errors and continue", QMessageBox::ActionRole);
                    QPushButton *btnContinue = msgBox.addButton("Ignore and continue", QMessageBox::ActionRole);
                    QPushButton *btnAbort = msgBox.addButton("Exit program", QMessageBox::ActionRole);
                    msgBox.exec();
                    if (msgBox.clickedButton() == btnSkipOpenError) {
                        bSkipOpenError = true;
                        continue;
                    } else if (msgBox.clickedButton() == btnContinue) {
                        continue;
                    } else if (msgBox.clickedButton() == btnAbort) {
                        exit(EXIT_FAILURE);
                    }
                }
                QTextStream in(&file);
                in.setCodec("UTF-8");
                QChar myqc;
                QString ingamename = "";
                QString myQCharToQString = "";
                unsigned int mycounter = 0;
                while (!in.atEnd()){
                    if(mycounter > 49){
                        break;
                    }
                    in >> myqc;
                    if(myqc.isPrint()){
                        if(myqc.isLetterOrNumber() || myqc.isPunct() || myqc.isSpace() || myqc == '|'){
                            myQCharToQString = myqc;
                            ingamename.append(myQCharToQString);
                        }
                    }
                    mycounter++;
                }

                if(ingamename.startsWith("HM3W", Qt::CaseInsensitive)){
                    ingamename.remove(0, 4);
                }

                //color codes begin with "|c" or "|C", and finishes with "|r", most of the times. An example is "|c00BBGGRRRRthisIsTheIngameMapNameColored|r"
                int colorcount = ingamename.count("|c",Qt::CaseInsensitive);
                for(int j = 0;j < colorcount; j++){
                    int indexOf = ingamename.indexOf("|c");
                    if(indexOf == -1){
                        indexOf = ingamename.indexOf("|C");
                    }
                    if(indexOf != -1){
                        //check if next 10 chars are hexadecimal
                        int indexOfPlus = indexOf + 10;
                        boolean bIsHexadecimal = true;
                        for(int i = indexOf+2;i<indexOfPlus;i++){
                            QChar c = ingamename.at(i);
                            if(!( c.isNumber() || c == 'a' || c == 'A' || c == 'b' || c == 'B' || c == 'c' || c == 'C' || c == 'd' || c == 'D' || c == 'e' || c == 'E' || c == 'f' || c == 'F')){
                                bIsHexadecimal = false;
                                break;
                            }
                        }
                        if(bIsHexadecimal){
                            ingamename.remove(indexOf, 10);
                            indexOf = ingamename.indexOf("|r", Qt::CaseInsensitive);
                            if(indexOf != -1){
                                ingamename.remove(indexOf, 2);
                            }
                        }
                    }
                }

                stringstream ss;
                ss << maxNumPlayers;
                string smaxNumPlayers = ss.str();
                counter++;
                query.exec(QString("INSERT INTO maps (ingamename,filename,maximumPlayers,rating,tags) VALUES ('%1','%2',%3,1,'none');").arg(ingamename.replace(QString("'"), QString("''"))).arg(filename).arg(QString::fromStdString(smaxNumPlayers)));
                //                qDebug()<< ingamename.replace(QString("'"), QString("''")) << " " << filename << " " << QString::fromStdString(smaxNumPlayers);
                //                if (query.exec(QString("INSERT INTO maps (ingamename,filename,maximumPlayers,rating,tags) VALUES ('%1','%2',%3,1,'none');").arg(ingamename.replace(QString("'"), QString("''"))).arg(filename).arg(QString::fromStdString(smaxNumPlayers)))){
                //                }
                //                else{
                //                    qDebug() << "insert error";
                //                    qDebug() << filename;
                //                    qDebug() << ingamename;
                //                }
            }
        }
    }
    label->hide();
    model->select();
    ui->view->show();
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,64);

    query.exec("END TRANSACTION");


    //  fix the slider of the scrollbar of qtableview getting resized and jumping up a bit when dragging the slider down
    // probably because sqllite does not support Query size. (QSqlDriver::hasFeature(QuerySize))
    while(model->canFetchMore()){
        model->fetchMore();
    }

    query.exec("PRAGMA synchronous = ON"); // back to default settings
    query.exec("PRAGMA journal_mode = DELETE");// back to default settings
}

