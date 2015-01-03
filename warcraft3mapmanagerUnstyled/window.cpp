#include <QtWidgets>
#include <QtSql>
#include <QSpinBox>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <list>
#include <stdio.h>
#include <cstring>
#include <sstream>

#include "window.h"
#include "ui_window.h"

#include "starratingdelegate.h"
#include "preventeditingdelegate.h"
#include "QtWaitingSpinner.h"

using namespace std;

Window::~Window()
{
    delete ui;
}
Window::Window(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_MinimumRating = 1;
    m_MinimumMaxPlayers = 1;
    m_MaxPlayers = 12;
    m_sFilterIngameName = "";
    m_sFilterFileName = "";

    m_spinner = new QtWaitingSpinner(40,40,7,200,ui->view);

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
    preventeditingdelegate *deleg = new preventeditingdelegate();
    ui->view->setItemDelegateForColumn(0, deleg);
    ui->view->setItemDelegateForColumn(1, deleg);
    ui->view->setItemDelegateForColumn(2, deleg);
    ui->view->setEditTriggers(QAbstractItemView::CurrentChanged); //single click creates editor to change cell
    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &))
            );

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataOfCellChangedSlot(const QModelIndex&, const QModelIndex&)));

    ui->btnLoad->setEnabled(false);
    connect(ui->btnLoad, SIGNAL(released()), this, SLOT(btnLoadSlot()));

    QFile file("savedWarcraft3MapsPath");
    if ( file.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&file);
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
    file.close();
    ui->lMapDirStr->setWordWrap(true);

    connect(ui->btnSetMapDir, SIGNAL(released()), this, SLOT(btnSetMapDirSlot()));

    connect(ui->btnResetFilters, SIGNAL(released()), this, SLOT(btnResetFiltersSlot()));

    connect(ui->btnCreateNewDB, SIGNAL(released()), this, SLOT(btnCreateNewDBSlot()));

    ui->sbMinimumRating->setMinimum(1);
    ui->sbMinimumRating->setMaximum(5);
    connect(ui->sbMinimumRating, SIGNAL(valueChanged(const int)), this, SLOT(sbMinimumRatingSlot(const int)));

    ui->sbMinimumMaxPlayers->setMinimum(1);
    ui->sbMinimumMaxPlayers->setMaximum(12);
    connect(ui->sbMinimumMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(sbMinimumMaxPlayersSlot(int)));

    ui->sbMaxPlayers->setMinimum(1);
    ui->sbMaxPlayers->setMaximum(12);
    ui->sbMaxPlayers->setValue(12);
    connect(ui->sbMaxPlayers, SIGNAL(valueChanged(const int)), this, SLOT(sbMaxPlayersSlot(int)));

    connect(ui->leFilterIngameName,SIGNAL(textChanged(const QString &)),this,SLOT(leFilterIngameNameSlot(const QString &)));

    connect(ui->leFilterFileName,SIGNAL(textChanged(const QString &)),this,SLOT(leFilterFileNameSlot(const QString &)));

    connect(ui->leFilterTags,SIGNAL(textChanged(const QString &)),this,SLOT(leFilterTagsSlot(const QString &)));

    ui->btnOpenInExplorer->setEnabled(false);
    connect(ui->btnOpenInExplorer, SIGNAL(released()), this, SLOT(btnOpenInExplorerSlot()));

    ui->btnMoveMap->setEnabled(false);
    connect(ui->btnMoveMap, SIGNAL(released()), this, SLOT(btnMoveMapSlot()));

    setContentsMargins(0,0,0,0);
    setMinimumWidth(500);
    setMinimumHeight(460);

    setWindowTitle(tr("Wc3 Map Manager"));
    setWindowIcon(QIcon(":/re/windowicon.ico"));
    QWidget::showMaximized();
}

void Window::resizeEvent(QResizeEvent* event){
    ui->view->setFixedWidth(event->size().width()-200);
    ui->view->setFixedHeight(event->size().height());
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);
}

void Window::onDataOfCellChangedSlot(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);
}
void Window::selectionChangedSlot(const QItemSelection &, const QItemSelection &)
{
    ui->btnOpenInExplorer->setEnabled(true);
    ui->btnMoveMap->setEnabled(true);
    disconnect(ui->view->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &))
               );
}
void Window::sbMinimumRatingSlot(const int i){
    m_MinimumRating = i;
    updateFilters();
}
void Window::sbMinimumMaxPlayersSlot(int i){
    if(i > m_MaxPlayers){
        m_MaxPlayers = i;
        ui->sbMaxPlayers->setValue(i);
    }
    m_MinimumMaxPlayers = i;
    updateFilters();
}
void Window::sbMaxPlayersSlot(int i){
    if(i < m_MinimumMaxPlayers){
        m_MinimumMaxPlayers = i;
        ui->sbMinimumMaxPlayers->setValue(i);
    }
    m_MaxPlayers = i;
    updateFilters();
}
void Window::leFilterIngameNameSlot(const QString &s){
    m_sFilterIngameName = s;
    m_sFilterIngameName.replace(QString("'"), QString("''"));
    updateFilters();
}
void Window::leFilterFileNameSlot(const QString &s){
    m_sFilterFileName = s;
    m_sFilterFileName.replace(QString("'"), QString("''"));
    updateFilters();
}
void Window::leFilterTagsSlot(const QString &s){
    m_sFilterTags = s;
    m_sFilterTags.replace(QString("'"), QString("''"));
    updateFilters();
}
void Window::updateFilters(){
    QStringList sl = m_sFilterTags.split(" ", QString::SkipEmptyParts);
    QString sqlTags;
    for (QStringList::iterator it = sl.begin();it != sl.end(); ++it) {
        sqlTags.append(QString(" and tags LIKE '%%1%'").arg(*it));
    }
    model->setFilter(QString("rating >= '%1' and maximumPlayers >= '%2' and maximumPlayers <= '%3' and ingamename LIKE '%%4%' and filename LIKE '%%5%'%6").arg(m_MinimumRating).arg(m_MinimumMaxPlayers).arg(m_MaxPlayers).arg(m_sFilterIngameName).arg(m_sFilterFileName).arg(sqlTags));
}
void Window::btnSetMapDirSlot()
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
void Window::btnResetFiltersSlot()
{
    m_MinimumRating = 1;
    m_MinimumMaxPlayers = 1;
    m_MaxPlayers = 12;
    m_sFilterIngameName = "";
    m_sFilterFileName = "";
    ui->sbMinimumMaxPlayers->setValue(1);
    ui->sbMaxPlayers->setValue(12);
    ui->leFilterIngameName->setText("");
    ui->leFilterFileName->setText("");
    ui->leFilterTags->setText("");
    updateFilters();
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);
}
void Window::btnCreateNewDBSlot(){
    if (! db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Database error! Please check if 'Warcraft3Maps.db.db' is write-protected");
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
}
void Window::btnOpenInExplorerSlot()
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
void Window::btnMoveMapSlot()
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
void Window::btnLoadSlot()
{
    m_spinner->showMaximized();
    m_spinner->start();

    QSqlQuery query(db);

    QDir *dir = new QDir(m_sMapDir);
    QDirIterator iterator(dir->absolutePath(), QDirIterator::Subdirectories);
    boolean bSkipOpenError = false;
    boolean bSkipOpenMaxPlayerError = false;
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.fileName();
            if (filename.endsWith(".w3x") || filename.endsWith(".w3m")){

                qApp->processEvents(QEventLoop::ExcludeUserInputEvents); //allows animated loading spinner

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
                if (query.exec(QString("INSERT INTO maps (ingamename,filename,maximumPlayers,rating,tags) VALUES ('%1','%2',%3,1,'none');").arg(ingamename.replace(QString("'"), QString("''"))).arg(filename).arg(QString::fromStdString(smaxNumPlayers)))){
                }
                //                else{
                //                    qDebug() << "insert error";
                //                    qDebug() << filename;
                //                    qDebug() << ingamename;
                //                }
            }
        }
    }
    m_spinner->finish();
    m_spinner->hide();
    model->select();
    ui->view->resizeColumnsToContents();
    ui->view->setColumnWidth(2,62);
}
