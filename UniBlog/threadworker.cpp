#include "threadworker.h"
#include "mysocket.h"
#include "File/FTPsocket.h"
#include <QDebug>
ThreadWorker::ThreadWorker()
{

}

void ThreadWorker::doTcpWorker()
{
    //qDebug()<<"TCP thread id = " << QThread::currentThreadId();
    if(MySocket::getTcpSocket()->isConnected())
        MySocket::getTcpSocket()->readyrecvData();
}

void ThreadWorker::doFtpWorker()
{
    if(FTPsocket::getTcpSocket()->isConnected()){
       FTPsocket::getTcpSocket()->readyrecvData();
    }
}

void ThreadWorker::doWriteWorker()
{

}

void ThreadWorker::loadUserAllBlog()
{
    qDebug()<<"load thread id = " << QThread::currentThreadId();
    QVector<blogSqlData> m_return;
    SqliteDataBase::getMySqlite()->queryAllBlogEdit(m_return);
    emit loadUserAllBlogFinished(m_return);
}
