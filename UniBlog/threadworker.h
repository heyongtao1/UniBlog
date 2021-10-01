#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>
#include "SqlDatabase/sqlitedatabase.h"
#include "myapp.h"
class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    ThreadWorker();

private slots:
    void doTcpWorker();
    void doFtpWorker();
    void doWriteWorker();
public slots:
    void loadUserAllBlog();
signals:
    void loadUserAllBlogFinished(const QVector<blogSqlData> m_return);
};

#endif // THREADWORKER_H
