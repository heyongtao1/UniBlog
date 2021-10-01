#include "globalapplocation.h"

GlobalApplication::GlobalApplication(int &argc,char **argv):
QApplication(argc,argv)
{
    m_currWinObj = new QObject(); //初始化鼠标事件发生时的当前所在窗口
    m_currWinObj = nullptr;
    //初始化定时器线程和定时器
    m_pThread = new QThread(this);
    m_pTimer = new QTimer();
    m_pTimer->moveToThread(m_pThread);
    //超时时间锁屏1分钟
    m_pTimer->setInterval(1000000);
    connect(m_pThread, SIGNAL(started()), m_pTimer, SLOT(start()));
    connect(m_pTimer, SIGNAL(timeout()),this,SLOT(onTimeOut()));
    //开启线程，调用定时器的start()
    m_pThread->start();

    m_myLockWindow = new LockScreenWidget(); //初始化锁屏窗口
    m_myLockWindow->m_unlockFlag = true; //初始化解除锁屏成功标志
}

GlobalApplication::~GlobalApplication()
{
    qDebug() << "~GlobalApplication() start";
    if(m_myLockWindow != nullptr){
        m_myLockWindow->deleteLater();
        m_myLockWindow = nullptr;
    }
    if(m_pTimer != nullptr){
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
    //彻底结束线程（单独的一个quit()无法彻底结束进程）
    m_pThread->requestInterruption();
    m_pThread->quit();
    m_pThread->wait();
    if(m_pThread != nullptr){
        m_pThread->deleteLater();
        m_pThread = nullptr;
    }
    qDebug() << "~GlobalApplication() end";
}

bool GlobalApplication::notify(QObject *obj, QEvent *e)
{
    //因为鼠标点击或滑动到任何窗口部件都会进入该函数，所以根据objectName()过滤窗口部件
    if(obj->objectName().left(12) == "BlogFunction") //cleanW mainW
    {
        //工控机是触摸屏，触摸屏幕和滑动屏幕都会点击屏幕，所以这里只检测鼠标点击事件
        if(e->type() == QEvent::MouseButtonPress) //不区分左右键按下了
        {
            //qDebug() << "objName:" << obj->objectName();
            qDebug()<<"press ";
            m_pThread->quit(); //线程结束后，则在此线程的定时器会自动停止计时
        }
        else if(e->type() == QEvent::MouseButtonRelease)
        {
            qDebug()<<"release ";
            m_pThread->start(); //线程开始，会发送started()信号，启动定时器的start()槽函数
        }
        m_currWinObj = obj; //获得鼠标事件发生时的当前所在窗口
    }
    //登录界面不响应锁屏
    if(obj->objectName() == "BlogLogin")
        m_pThread->quit();
    return QApplication::notify(obj,e);
}

//超时锁屏槽函数
void GlobalApplication::onTimeOut()
{
    //m_unlockFlag为false,表示锁屏窗口未关闭，则即使再超时也不会再显示锁屏窗口
    if(m_myLockWindow->m_unlockFlag == true) //当密码输入正确后，关闭锁屏窗口，则会将m_unlockFlag设置为true
    {
        //static_cast<QWidget *>(m_currWinObj)->hide();
        qDebug()<<"锁屏";
        //创建锁屏窗口对象，并显示该窗口
        m_myLockWindow = new LockScreenWidget();
        //阻塞除当前窗体之外的所有的窗体
        m_myLockWindow->setWindowModality(Qt::ApplicationModal);
        //m_myLockWindow = new Widget_Lock(static_cast<QWidget *>(m_currWinObj));
        m_myLockWindow->show();
        //这行代码要放在最后，因为在构建对象时，会自动设置m_unlockFlag，所以在构建后要复位m_unlockFlag
        m_myLockWindow->m_unlockFlag = false;
    }
    else
        qDebug()<<"重复锁屏";
}
