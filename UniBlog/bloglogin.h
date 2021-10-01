#ifndef BLOGLOGIN_H
#define BLOGLOGIN_H

#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QWidget>
#include <event/KeyBoard/keyboard.h>
#include "blogfunction.h"
#include "mysocket.h"
#include "basewidget/LoadingDialog.h"
#include "SqlDatabase/sqlitedatabase.h"
//#include <QtWebEngineWidgets/qwebengineview.h>
namespace Ui {
class BlogLogin;
}

class BlogLogin : public QWidget
{
    Q_OBJECT

public:
    explicit BlogLogin(QWidget *parent = 0);
    ~BlogLogin();
    void setBlogLoginWidget();
    void readLoginWidgetQss();
    //停止加载界面显示
    void stoploadingDialog();
    //控制窗口特效显示
    void controlWidgetShow();
    //网络信号接口
    void netWorkSingal();
    //加载用户所有博客
    void loadingUserAllBlog();
    bool m_unlockFlag;
protected:
    QPoint mousePoint;
    bool m_mousePressed;
    void mouseMoveEvent(QMouseEvent *e);//鼠标移动事件
    void mousePressEvent(QMouseEvent *e);//鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *);//鼠标松开事件

private slots:
    void on_closeBt_clicked();

    void on_settingBt_clicked();

    void on_SettingcloseBt_clicked();

    void on_loginBt_clicked();

    void animationEndWidgetClose();

    void on_animation_stateChanged(QAbstractAnimation::State newState,
                                   QAbstractAnimation::State oldState);
    void on_registerBt_clicked();

    void connectTCPServerSignal(bool state);

    void connectFTPServerSignal(bool state);

    void userOnSearch(bool);

    void PassOnSearch(bool);
private slots:
    //服务器返回信息状态的槽函数
    void loginMag(bool state);
private:
    Ui::BlogLogin *ui;
    LoadingDialog *loading;
    QPropertyAnimation *animation;
    BlogFunction *blogfun;
    KeyBoard *im;
};

#endif // BLOGLOGIN_H
