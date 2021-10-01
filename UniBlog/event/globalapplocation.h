#ifndef GLOBALAPPLICATION_H
#define GLOBALAPPLICATION_H

#include <QApplication>
#include <QMouseEvent>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "basewidget/lockscreenwidget.h"
/**
 * @brief The GlobalApplication class
 * 锁屏模块
 */
class GlobalApplication : public QApplication
{
    Q_OBJECT

public:
     GlobalApplication(int&argc,char **argv);
     ~GlobalApplication();

     bool notify(QObject*, QEvent *);

private slots:
    void onTimeOut(); //超时锁屏槽函数

private:
    LockScreenWidget *m_myLockWindow; //锁屏窗口
    QObject *m_currWinObj; //鼠标事件发生时的当前所在窗口

    QTimer *m_pTimer; //定时器
    QThread *m_pThread; //定时器线程
};

#endif // GLOBALAPPLICATION_H
