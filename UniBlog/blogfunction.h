#ifndef BLOGFUNCTION_H
#define BLOGFUNCTION_H

#include <QWidget>
#include <QButtonGroup>
#include <QListWidget>
#include <File/filemag.h>
#include "basewidget/mywidget.h"
#include "basewidget/previewblogwidget.h"
#include "blogtext/blogtext.h"
#include "event/globalapplocation.h"
#include "mysocket.h"
#include "SqlDatabase/sqlitedatabase.h"
namespace Ui {
class BlogFunction;
}

class BlogFunction : public MyWidget
{
    Q_OBJECT

public:
    explicit BlogFunction(QWidget *parent = 0);
    ~BlogFunction();
    void setBlogLoginWidget();
    void readLoginWidgetQss();
    void setQPushButtonGroup();
    void initWidget();
    //CLog类的日志信息输出函数封装
    void outLogInfo(char* logtext);
    //服务器返回的消息信号槽函数连接
    void serverBackSignalConnect();
    //加载推荐博客文章
    void loadingRecomBlogEdit();
    //向服务器发送请求用户所有博客
    void loadingUserAllBlog();
    //查看文章界面的选中后特效
    void findblogWidgetAnimation();
private slots:
    void SltChangePages(int index);
    void AboutSltChangePages(int index);
    void on_PostArticlesBt_clicked();
    void on_AllCreateBlogBt_clicked();
    void on_quitBlogBt_clicked();
    void animationFinishedWidgetClose();
    void recvloadUserAllBlog(const QVector<blogSqlData> m_return);
    void recvselectReplaceUserHead(QString headPath);
signals:
    void CloseblogfunWidgetSingal();
    void activeCloseSoftware();
private slots:
    //与服务器返回的消息连接的槽函数
    void recvServerPubBlogEditSignal(bool state);
    void recvServerUserAllBlogEditSignal(QJsonValue dataval);
    void recvServerFindBlogEditSignal(QJsonValue dataval);
    void recvServerRecomBlogEditSignal(QJsonValue dataval);
    void recvServerwritePicFileSignal(QJsonValue dataval);
    void recvServerDownloadFileSignal(QJsonValue dataval);
    //ui控件槽函数
    void on_findBlogEditBt_clicked();

    void on_closeBt_clicked();

    void on_HelpOurBt_clicked();

    void on_qssBlackBt_clicked();

    void on_selectPathBt_clicked();

    void on_sendFileBt_clicked();
    void on_WidgetMaxBt_clicked();

    void on_WidgetMinBt_clicked();

    void on_userHeadBt_clicked();

private:
    Ui::BlogFunction *ui;
    QButtonGroup *m_funbuttonGroup;
    QButtonGroup *m_aboutButtonGroup;
    QList<BlogText *> m_allblogtext;
    QListWidgetItem *m_removeitem;
    bool m_bIsMenu;
    PreviewBlogWidget *m_pEmergancy;
    QThread *loadThread;
    //最后一次上传资源的对象
    FileMag *filemsg;
private:
    //控件的控制变量
    bool widgetChange;
};

#endif // BLOGFUNCTION_H
