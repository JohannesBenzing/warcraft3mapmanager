#ifndef MODELSQLITE_H
#define MODELSQLITE_H

#include <QtSql>
#include <string>

/**
     * @brief stores data read out of warcraft 3 maps inside a SQlite database.
     * Also creates a file named "savedWarcraft3MapsPath" to store last used directory in which maps have been searched
     */

class ModelSqlite
{
public:
    ModelSqlite();

    QString createIfNotExistsSavedWarcraft3MapsPathAndReturnIt() const;
    QSqlTableModel * getModel() const;
    void createNewDB();
    /**
         * @brief Goes recursively through all directories starting at argument-directory and opens all warcraft3map files(.w3x , .w3m)
         * and read saves values like filename, ingamemapname, maximum number of players into the database.
         */
    void loadMapDataRecursivelyFromDir(QString sDir);
    void setFilters(QString filters);

private:
    QSqlDatabase m_db;
    QSqlTableModel *m_model;
};

#endif // MODELSQLITE_H
