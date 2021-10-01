#include "timerthread.h"
#include "FTPsocket.h"
TimerThread::TimerThread() : QThread()
{

}

TimerThread::~TimerThread()
{
    /*
    m_pTimer->stop();
    if(m_pTimer != nullptr){
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
    */
}

void TimerThread::run()
{
    /*
    m_pTimer = new QTimer();
    m_pTimer->setInterval(1000);
    connect(m_pTimer, &QTimer::timeout, this, &TimerThread::timeoutSlot,Qt::DirectConnection);
    m_pTimer->start(200);
    this->exec();
    */
}

void TimerThread::timeoutSlot()
{
    /*
    FTPsocket::getTcpSocket()->autoConnectToHost();
    if(FTPsocket::getTcpSocket()->isConnected()){
       FTPsocket::getTcpSocket()->readyrecvData();
       m_pTimer->setInterval(30);
    }else{
       m_pTimer->setInterval(200);
    }
    //m_pTimer->stop();
    //qDebug() << "timerthread id = " << QThread::currentThreadId();
    */
}
