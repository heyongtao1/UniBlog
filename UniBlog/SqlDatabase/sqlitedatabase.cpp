#include "sqlitedatabase.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
SqliteDataBase * SqliteDataBase::mySqlite = new SqliteDataBase("blog.db");
SqliteDataBase::deleteMySqlObj SqliteDataBase::deletesqlite;
SqliteDataBase::SqliteDataBase(QString dbName)
{
    //判断数据库是否已连接
    if(QSqlDatabase::contains("qt_sql_default_connection"))
       sqlitedb = QSqlDatabase::database("qt_sql_default_connection");
    else
       sqlitedb = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名称
    sqlitedb.setDatabaseName(dbName);
    if(!sqlitedb.open())
       qDebug()<<":::::::Failed to open database!:::::::"<<sqlitedb.lastError();
    else
       qDebug()<<":::::::Open database!:::::::";
    createTable();
}
SqliteDataBase::~SqliteDataBase()
{
    sqlitedb.close();
}
void SqliteDataBase::createTable()
{
    QString query = "CREATE TABLE IF NOT EXISTS blog(userid INTEGER ,blogEditid INTEGER, blogText Text NOT NULL, blogHead Text NOT NULL, pubtime Text NOT NULL , primary key(userid,blogEditid))";
    QSqlQuery sqlquery(sqlitedb);
    if(!sqlquery.exec(query)){
        qDebug()<< "createTable fail  " << sqlquery.lastError();
    }else{
        qDebug()<< "createTable success";
    }
}
void SqliteDataBase::insertTable(const blogSqlData &query)
{
    QSqlQuery sqlQuery(sqlitedb);
    QString sql = QString("insert into blog values(%1,%2,'%3','%4','%5')").arg(query.userid).arg(query.blogEditid).arg(query.blogText).arg(query.blogHead).arg(query.publtime);
    qDebug()<<sql;
    if(!sqlQuery.exec(sql))
    {
        qDebug()<<"Failed to insert table:"<<sqlQuery.lastError();
    }
    else
    {
        qDebug()<<"success to insert";
    }
}

void SqliteDataBase::queryAllBlogEdit(QVector<blogSqlData> &m_return)
{
   QSqlQuery sqlQuery(sqlitedb);
   QString query = QString("select * from blog where userid=%1").arg(MyApp::userid);
   if(!sqlQuery.exec(query))
   {
       qDebug()<<"Failed to insert table:"<<sqlQuery.lastError();
   }
   else
   {
       while(sqlQuery.next())
       {
            blogSqlData m_blogSD;
            m_blogSD.userid = sqlQuery.value(0).toInt();
            m_blogSD.blogEditid = sqlQuery.value(1).toInt();
            m_blogSD.blogText = sqlQuery.value(2).toString();
            m_blogSD.blogHead = sqlQuery.value(3).toString();
            m_blogSD.publtime = sqlQuery.value(4).toString();
            m_return.push_back(m_blogSD);
       }
   }
}
