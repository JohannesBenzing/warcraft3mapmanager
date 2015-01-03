#include "ModelSqlite.h"

#include <QtWidgets>
#include <QtSql>

#include <fstream>
#include <sstream>

using namespace std;

ModelSqlite::ModelSqlite()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("Warcraft3Maps.db");
    if (! m_db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Database error! Please check if 'Warcraft3Maps.db.db' is write-protected");
        msgBox.addButton("Exit program", QMessageBox::ActionRole);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    else{
        QSqlQuery query(m_db);
        if (! query.exec("CREATE TABLE IF NOT EXISTS maps (ingamename VARCHAR(45) NULL,filename VARCHAR(99) NULL,maximumPlayers INT NULL,rating INT NULL,tags VARCHAR(99) NULL);")){
            QMessageBox msgBox;
            //            qDebug() << query.lastError();
            msgBox.setText("Database error! Please check if 'Warcraft3Maps.db' is write-protected");
            msgBox.addButton("Exit program", QMessageBox::ActionRole);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    }

    m_model = new QSqlTableModel();
    m_model->setTable("maps");
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_model->select();

    m_model->setHeaderData(0, Qt::Horizontal, "Ingame-name");
    m_model->setHeaderData(1, Qt::Horizontal, "File-name");
    m_model->setHeaderData(2, Qt::Horizontal, "Max.\n players");
    m_model->setHeaderData(3, Qt::Horizontal, "Rating");
    m_model->setHeaderData(4, Qt::Horizontal, "Tags");
}

QString ModelSqlite::createIfNotExistsSavedWarcraft3MapsPathAndReturnIt() const {
    QFile pathfile("savedWarcraft3MapsPath");
    if ( pathfile.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&pathfile);
        QString line = in.readLine();
        pathfile.close();
        return line;
    }
    return QString("");
}

QSqlTableModel * ModelSqlite::getModel() const {
    return m_model;
}

void ModelSqlite::createNewDB(){
    if (! m_db.open()) {
        QMessageBox msgBox;
        msgBox.setText("Database error! Please check if 'Warcraft3Maps.db' is write-protected");
        msgBox.addButton("Exit program", QMessageBox::ActionRole);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
    QSqlQuery query(m_db);
    if(query.exec(QString("DROP TABLE IF EXISTS maps;"))){
        query.exec(QString("CREATE TABLE maps (ingamename VARCHAR(45) NULL,filename VARCHAR(99) NULL,maximumPlayers INT NULL,rating INT NULL,tags VARCHAR(99) NULL);"));
    }
    m_model->setTable("maps");
    m_model->select();
    m_model->setHeaderData(0, Qt::Horizontal, "Ingame-name");
    m_model->setHeaderData(1, Qt::Horizontal, "File-name");
    m_model->setHeaderData(2, Qt::Horizontal, "Max.\n players");
    m_model->setHeaderData(3, Qt::Horizontal, "Rating");
    m_model->setHeaderData(4, Qt::Horizontal, "Tags");
}

void ModelSqlite::loadMapDataRecursivelyFromDir(QString sDir){
    QSqlQuery query(m_db);

    //  more performance but might corrupte database on crash
    //  http://stackoverflow.com/questions/1711631/how-do-i-improve-the-performance-of-sqlite
    //  much faster!!
    query.exec("PRAGMA synchronous = OFF");
    query.exec("PRAGMA journal_mode = MEMORY");
    query.exec("BEGIN TRANSACTION");

    QDir *dir = new QDir(sDir);
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
    m_model->select();

    query.exec("END TRANSACTION");

    //  fix the slider of the scrollbar of qtableview getting resized and jumping up a bit when dragging the slider down
    // probably because sqllite does not support Query size. (QSqlDriver::hasFeature(QuerySize))
    while(m_model->canFetchMore()){
        m_model->fetchMore();
    }

    query.exec("PRAGMA synchronous = ON"); // back to default settings
    query.exec("PRAGMA journal_mode = DELETE");// back to default settings
}

void ModelSqlite::setFilters(QString filters){
    m_model->setFilter(filters);
}
