#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include "../myapp.h"

class SqliteDataBase : public QObject
{
    Q_OBJECT
public:
    static SqliteDataBase *getMySqlite(){
        return mySqlite;
    }
    class deleteMySqlObj{
    public:
          deleteMySqlObj(){

          }
          ~deleteMySqlObj(){
              if(mySqlite != nullptr){
                  mySqlite->deleteLater();
                  mySqlite = nullptr;
                  qDebug()<< "~mySqlite";
              }
          }
    };
public:
    void createTable();
    void insertTable(const blogSqlData &query);
    void queryAllBlogEdit(QVector<blogSqlData> &m_return);
    //void updataTable(QString &sql);
    //void delData(QString &sql);
    //bool select(QString tablename,QString username,QString pwd);
private:
    SqliteDataBase(QString dbName);
    ~SqliteDataBase();
    QSqlDatabase sqlitedb;
    static SqliteDataBase *mySqlite;
    static deleteMySqlObj deletesqlite;
};

#endif // SQLITEDATABASE_H
