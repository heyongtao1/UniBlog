#include <QGraphicsDropShadowEffect>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextDocument>
#include <QTextFrame>
#include <QFileDialog>
#include <QDateTime>
#include <iostream>
#include "event/base64.h"
#include "blogfunction.h"
#include "ui_blogfunction.h"
#include "basewidget/animationstackedwidget.h"
#include "basewidget/previewblogwidget.h"
#include "basewidget/customwidget.h"
#include "myapp.h"
#include "mysocket.h"
#include "blogSystem/commonhelper.h"
#include "File/filemag.h"
#include "basewidget/filemanager.h"
#include "File/FTPsocket.h"
#include "user/userheadwidget.h"
using namespace std;
BlogFunction::BlogFunction(QWidget *parent) :
    MyWidget(parent),
    ui(new Ui::BlogFunction)
{
    ui->setupUi(this);
    m_removeitem=new QListWidgetItem();
    m_pEmergancy = new PreviewBlogWidget();
    setBlogLoginWidget();
    readLoginWidgetQss();
    setQPushButtonGroup();
    initWidget();
    serverBackSignalConnect();
    //子线程加载本地数据
    qRegisterMetaType<QVector<blogSqlData> >("QVector<blogSqlData> ");//注册该类型在线程使用
    ThreadWorker worker;
    loadThread = new QThread();
    worker.moveToThread(loadThread);
    //任务结束发射信号，UI处理显示数据
    connect(&worker,&ThreadWorker::loadUserAllBlogFinished,this,&BlogFunction::recvloadUserAllBlog);
    //线程任务完毕后销毁
    connect(loadThread,&QThread::finished,&worker,&QObject::deleteLater);
    //线程开始和启动加载本地博客关联
    connect(loadThread,&QThread::started,&worker,&ThreadWorker::loadUserAllBlog,Qt::DirectConnection);
    loadThread->start();
    //加载推荐博客文章
    loadingRecomBlogEdit();
}

BlogFunction::~BlogFunction()
{
    if(m_removeitem != nullptr){
        delete m_removeitem;
        m_removeitem = nullptr;
    }
    loadThread->quit();
    loadThread->wait();
    loadThread->deleteLater();
    loadThread = nullptr;
    m_allblogtext.clear();
    m_funbuttonGroup->deleteLater();
    m_funbuttonGroup = nullptr;
    m_aboutButtonGroup->deleteLater();
    m_aboutButtonGroup = nullptr;
    m_pEmergancy->deleteLater();
    m_pEmergancy = nullptr;
    qDebug()<< " ~BlogFunction ~m_buttonGroup";
    delete ui;
}

/**
 * @brief BlogFunction::setBlogLoginWidget
 */
void BlogFunction::setBlogLoginWidget()
{
    //实现QListWidget多列显示
    ui->resourlistWidget->setWrapping(true);						//自动换行
    ui->resourlistWidget->setFlow(QListView::LeftToRight);		//从左到右布局
    //设置窗体透明
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置无边框
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor("#444444"));
    //设置阴影圆角
    shadow->setBlurRadius(30);
    //给嵌套QWidget设置阴影
    ui->Blogfunwidget->setGraphicsEffect(shadow);
    //给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
    ui->verticalLayout->setMargin(10);
    ui->tabWidget->setStyleSheet("QTabBar::tab {border: none;border-bottom: 2px solid #3c3e42;min-width: 10px;margin-right: 20px;"
                                          "padding-left:20px;padding-right:20px;padding-top:5px;padding-bottom:5px;color:#686a6e;}"
                                          "QTabBar::tab:hover {background: #3a3a3f;}"
                                          "QTabBar::tab:selected {border-color: #3a3a3f;color:#dcdde4;border-bottom-color: #dcdde4;}");
}
/**
 * @brief BlogFunction::readLoginWidgetQss
 */
void BlogFunction::readLoginWidgetQss()
{
    //这是在Qt的资源下的文件,可以不用在资源下
    QFile file(":/resources/qss/blog.qss");
    //只读方式打开文件
    file.open(QFile::ReadOnly);
    //读取文件的所有内容，并转换成QString类型
    QString styleSheet = tr(file.readAll());
    //当前窗口设置样式表
    ui->Blogfunwidget->setStyleSheet(styleSheet);
    file.close();
}
/**
 * @brief BlogFunction::setQPushButtonGroup
 */
void BlogFunction::setQPushButtonGroup()
{
    m_funbuttonGroup = new QButtonGroup(this);
    m_funbuttonGroup->addButton(ui->AllCreateBlogBt, 0);
    m_funbuttonGroup->addButton(ui->NewCreateBlogBt, 1);
    m_funbuttonGroup->addButton(ui->SendMesgBt, 2);
    m_funbuttonGroup->addButton(ui->commendBlogBt, 3);
    m_funbuttonGroup->addButton(ui->findBlogTextBt, 4);
    m_funbuttonGroup->addButton(ui->selfChatBoxBt, 5);
    m_funbuttonGroup->addButton(ui->HelpOurBt, 6);
    connect(m_funbuttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(SltChangePages(int)));

    m_aboutButtonGroup = new QButtonGroup(this);
    m_aboutButtonGroup->addButton(ui->updatePasswordBt, 0);
    m_aboutButtonGroup->addButton(ui->settingBt, 1);
    m_aboutButtonGroup->addButton(ui->widgetStyBt, 2);
    m_aboutButtonGroup->addButton(ui->pushButton_6, 3);
    m_aboutButtonGroup->addButton(ui->pushButton_7, 4);
    m_aboutButtonGroup->addButton(ui->pushButton_8, 5);
    connect(m_aboutButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(AboutSltChangePages(int)));
}
void BlogFunction::initWidget()
{
    //隐藏ListWidget的滚动条
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
/**
 * @brief BlogFunction::outLogInfo
 * @param logtext 输出文本信息
 * CLog类自定义输出日志信息
 */
void BlogFunction::outLogInfo(char* logtext)
{
    /*
    CLog::LogConfig logConfig;
    logConfig.isRecord2File = true;
    logConfig.level = 0;
    CLog::init(logConfig);
    CLOG_INFO(logtext);
    */
}
/**
 * @brief BlogFunction::serverBackSignalConnect
 */
void BlogFunction::serverBackSignalConnect()
{
     connect(MySocket::getTcpSocket(),SIGNAL(pubBlogEditMagSignal(bool)),this,SLOT(recvServerPubBlogEditSignal(bool)));
     connect(MySocket::getTcpSocket(),SIGNAL(userAllBlogEditMagSignal(QJsonValue)),this,SLOT(recvServerUserAllBlogEditSignal(QJsonValue)));
     connect(MySocket::getTcpSocket(),SIGNAL(findBlogEditMagSignal(QJsonValue)),this,SLOT(recvServerFindBlogEditSignal(QJsonValue)));
     connect(MySocket::getTcpSocket(),SIGNAL(recomBlogEditMsgSignal(QJsonValue)),this,SLOT(recvServerRecomBlogEditSignal(QJsonValue)));
     connect(FTPsocket::getTcpSocket(),SIGNAL(picMsgSignal(QJsonValue)),this,SLOT(recvServerwritePicFileSignal(QJsonValue)));
     connect(FTPsocket::getTcpSocket(),SIGNAL(downloadFileSignal(QJsonValue)),this,SLOT(recvServerDownloadFileSignal(QJsonValue)));
}
/**
 * @brief BlogFunction::loadingRecomBlogEdit
 * 加载推荐博客文章
 */
void BlogFunction::loadingRecomBlogEdit()
{
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);

    object.insert("data",data_obj);
    object.insert("type",RecomBlogMagType::RECOMBLOG_MAG);

    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
}

void BlogFunction::loadingUserAllBlog()
{
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);

    object.insert("data",data_obj);
    object.insert("type",LoadBlogMagType::LOADBLOG_MAG);

    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据 ")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
}

/**
 * @brief BlogFunction::SltChangePages
 * 界面切换
 * @param index
 */
void BlogFunction::SltChangePages(int index)
{
    static int s_nIndex = 0;
    if (index == s_nIndex) return;

    ui->stackedWidget->setLength(ui->stackedWidget->width(),
                                     index > s_nIndex
                                     ? AnimationStackedWidget::LeftToRight
                                     : AnimationStackedWidget::RightToLeft);
    ui->stackedWidget->start(index);
    s_nIndex = index;
}

void BlogFunction::AboutSltChangePages(int index)
{
    static int s_nIndex = 0;
    if (index == s_nIndex) return;

    ui->AboutstackedWidget->setLength(ui->AboutstackedWidget->width(),
                                     index > s_nIndex
                                     ? AnimationStackedWidget::LeftToRight
                                     : AnimationStackedWidget::RightToLeft);
    ui->AboutstackedWidget->start(index);
    s_nIndex = index;
}

/**
 * @brief BlogFunction::on_PostArticlesBt_clicked
 * 发布文章
 */
void BlogFunction::on_PostArticlesBt_clicked()
{
    QString headtext = ui->headlineEdit->text();
    QString alltextinfo = ui->NewCreatetextEdit->toPlainText();
    if(headtext.count()==0||alltextinfo.count()==0)
    {
        CMessageBox::Infomation(this, QStringLiteral("提示"), QStringLiteral("文章标题或内容不能为空！"));
        return ;
    }
    //获取当前发布时间
    QString currtime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
    BlogText *blogtext = new BlogText("UniBlog",currtime,"",headtext,1,alltextinfo);
    m_allblogtext.append(blogtext);

    PreviewBlogWidget *previewblogwidget = new PreviewBlogWidget();
    previewblogwidget->setAllUiWidget(MyApp::userHead,"UniBlog",currtime,headtext,alltextinfo);
    connect(previewblogwidget->getPreviewBt(),&QPushButton::clicked,this,[=](){
        ui->text->setText(alltextinfo);
        ui->headtext->setText(headtext);
        ui->timelable->setText(currtime);
        SltChangePages(4);
    });

    //缓存到本地数据库
    blogSqlData m_blogSql;
    m_blogSql.userid = MyApp::userid;
    m_blogSql.blogEditid = ui->listWidget->count();
    m_blogSql.blogText = alltextinfo;
    m_blogSql.blogHead = headtext;
    m_blogSql.publtime = currtime;
    SqliteDataBase::getMySqlite()->insertTable(m_blogSql);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(previewblogwidget->width(),previewblogwidget->height()));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,previewblogwidget);
    ui->listWidget->update();
    m_removeitem = item;
    //发送博客数据到服务器
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);
    data_obj.insert("blogtext",alltextinfo);
    data_obj.insert("bloghead",headtext);
    data_obj.insert("pubtime",currtime);

    object.insert("data",data_obj);
    object.insert("type",BlogMagType::BLOG_MAG);

    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
    //清空输入框
    ui->NewCreatetextEdit->setText("");
    ui->headlineEdit->setText("");
}
/**
 * @brief BlogFunction::on_AllCreateBlogBt_clicked
 */
void BlogFunction::on_AllCreateBlogBt_clicked()
{

}
/**
 * @brief BlogFunction::on_quitBlogBt_clicked
 */
void BlogFunction::on_quitBlogBt_clicked()
{
    emit CloseblogfunWidgetSingal();
}

void BlogFunction::animationFinishedWidgetClose()
{
    //m_pEmergancy->hide();
    //ui->stackedWidget->setGeometry(200,38,791,651);
}

void BlogFunction::recvloadUserAllBlog(const QVector<blogSqlData> m_return)
{
    if(m_return.count() == 0){
        qDebug()<<"blog count = 0";
        //本地没有缓存数据，向服务器发送请求数据
        loadingUserAllBlog();
        return ;
    }else
         qDebug()<<"blog count = "<<m_return.count();
    for(int i=0;i<m_return.count();i++){
        PreviewBlogWidget *previewblogwidget = new PreviewBlogWidget();
        previewblogwidget->setAllUiWidget(MyApp::userHead,"UniBlog",m_return[i].publtime,m_return[i].blogHead,m_return[i].blogText);
        connect(previewblogwidget->getPreviewBt(),&QPushButton::clicked,this,[=](){
            ui->text->setText(m_return[i].blogText);
            ui->headtext->setText(m_return[i].blogHead);
            ui->timelable->setText(m_return[i].publtime);
            SltChangePages(4);
        });
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(previewblogwidget->width(),previewblogwidget->height()));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,previewblogwidget);
        ui->listWidget->update();
    }
    SltChangePages(0);
}

void BlogFunction::recvselectReplaceUserHead(QString headPath)
{
    ui->userHeadBt->setStyleSheet(QString("border-image: url(%1);").arg(headPath));
    MyApp::userHead = headPath;
    //更新所有头像控件
    int row=0;
    while(row<(ui->listWidget->count()))
    {
        QWidget *itemW = ui->listWidget->itemWidget(ui->listWidget->item(row));
        QList<QLabel*> labelList = itemW->findChildren<QLabel *>();
        foreach(QLabel *label, labelList)
        {
            qDebug()<< label->text();
            if(label->objectName() == "userhead")
                label->setPixmap(QPixmap(MyApp::userHead));
        }
        row++;
    }
}
//=========================================================================
void BlogFunction::recvServerPubBlogEditSignal(bool state)
{
    if(state){
        qDebug() << "recvServerPubBlogEditSignal(true)";
        CMessageBox::Infomation(this, QStringLiteral("提示"),QStringLiteral("文章发布成功！"));
    }else{
        qDebug() << "recvServerPubBlogEditSignal(false)";
        CMessageBox::Infomation(this, QStringLiteral("提示"), QStringLiteral("文章发布失败！"));
        ui->listWidget->removeItemWidget(m_removeitem);
    }
}

void BlogFunction::recvServerUserAllBlogEditSignal(QJsonValue dataVal)
{
    qDebug() <<"recvServerUserAllBlogEditSignal(QJsonValue &dataVal)";
    if(dataVal.isArray()){
        QJsonArray blogEditArray = dataVal.toArray();
        for(int i=0;i<blogEditArray.count();i++){
            int blogEditid = blogEditArray.at(i).toObject().value("blogeditid").toInt();
            QString blogText = blogEditArray.at(i).toObject().value("blogtext").toString();
            QString blogHead = blogEditArray.at(i).toObject().value("bloghead").toString();
            QString pubtime = blogEditArray.at(i).toObject().value("pubtime").toString();

            PreviewBlogWidget *previewblogwidget = new PreviewBlogWidget();
            previewblogwidget->setAllUiWidget(MyApp::userHead,"UniBlog",pubtime,blogHead,blogText);
            connect(previewblogwidget->getPreviewBt(),&QPushButton::clicked,this,[=](){
                ui->text->setText(blogText);
                ui->headtext->setText(blogHead);
                ui->timelable->setText(pubtime);
                SltChangePages(4);
                findblogWidgetAnimation();
            });
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(previewblogwidget->width(),previewblogwidget->height()));
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item,previewblogwidget);
            ui->listWidget->update();
        }
    }
}
/**
 * @brief BlogFunction::recvServerFindBlogEditSignal
 * @param dataval
 */
void BlogFunction::recvServerFindBlogEditSignal(QJsonValue dataVal)
{
    qDebug() <<"recvServerFindBlogEditSignal(QJsonValue &dataVal)";
    if(dataVal.isArray()){
        QJsonArray blogEditArray = dataVal.toArray();
        if(blogEditArray.count() == 0){
            CMessageBox::Infomation(this, QStringLiteral("查找提示"), QStringLiteral("含有该关键字的文章不存在"));
            return ;
        }
        for(int i=0;i<blogEditArray.count();i++){
            int userid = blogEditArray.at(i).toObject().value("userid").toInt();
            int blogEditid = blogEditArray.at(i).toObject().value("blogeditid").toInt();
            QString blogText = blogEditArray.at(i).toObject().value("blogtext").toString();
            QString blogHead = blogEditArray.at(i).toObject().value("bloghead").toString();
            QString pubtime = blogEditArray.at(i).toObject().value("pubtime").toString();

            PreviewBlogWidget *previewblogwidget = new PreviewBlogWidget();
            previewblogwidget->setAllUiWidget(MyApp::userHead,"UniBlog",pubtime,blogHead,blogText);
            connect(previewblogwidget->getPreviewBt(),&QPushButton::clicked,this,[=](){
                ui->text->setText(blogText);
                ui->headtext->setText(blogHead);
                ui->timelable->setText(pubtime);
                SltChangePages(4);
                findblogWidgetAnimation();
            });
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(previewblogwidget->width(),previewblogwidget->height()));
            ui->findlistWidget->addItem(item);
            ui->findlistWidget->setItemWidget(item,previewblogwidget);
            ui->findlistWidget->update();
        }
        SltChangePages(3);
    }
}

void BlogFunction::recvServerRecomBlogEditSignal(QJsonValue dataVal)
{
    qDebug() <<"recvServerRecomBlogEditSignal(QJsonValue &dataVal)";
    if(dataVal.isArray()){
        QJsonArray blogEditArray = dataVal.toArray();
        if(blogEditArray.count() == 0){
            //CMessageBox::Infomation(this, QStringLiteral("推荐提示"), QStringLiteral("没有可推荐博客文章"));
            return ;
        }
        for(int i=0;i<blogEditArray.count();i++){
            int userid = blogEditArray.at(i).toObject().value("userid").toInt();
            int blogEditid = blogEditArray.at(i).toObject().value("blogeditid").toInt();
            QString blogText = blogEditArray.at(i).toObject().value("blogtext").toString();
            QString blogHead = blogEditArray.at(i).toObject().value("bloghead").toString();
            QString pubtime = blogEditArray.at(i).toObject().value("pubtime").toString();

            PreviewBlogWidget *previewblogwidget = new PreviewBlogWidget();
            previewblogwidget->setAllUiWidget(MyApp::userHead,"UniBlog",pubtime,blogHead,blogText);
            connect(previewblogwidget->getPreviewBt(),&QPushButton::clicked,this,[=](){
                ui->text->setText(blogText);
                ui->headtext->setText(blogHead);
                ui->timelable->setText(pubtime);
                SltChangePages(4);
                findblogWidgetAnimation();
            });
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(previewblogwidget->width(),previewblogwidget->height()));
            ui->recomlistWidget->addItem(item);
            ui->recomlistWidget->setItemWidget(item,previewblogwidget);
            ui->recomlistWidget->update();
        }
        SltChangePages(3);
        qDebug()<<"recv blog success";
    }
}

void BlogFunction::recvServerwritePicFileSignal(QJsonValue dataval)
{
    CMessageBox::Infomation(this, QStringLiteral("上传资源"),QStringLiteral("上传成功！"));
    qDebug() <<"recvServerwritePicFileSignal(QJsonValue &dataVal)";
    if(dataval.isObject()){

            QJsonObject pic = dataval.toObject();
            int resourceid = pic.value("resourceid").toInt();
            /*
            unsigned long filesize = static_cast<unsigned long>(pic.value("filesize").toInt());
            const string filename = pic.value("filename").toString().toStdString();
            const string fromText = pic.value("filetext").toString().toStdString();
            unsigned char *decodeText = new unsigned char[filesize];
            CBase64::Decode(fromText,decodeText,&filesize);
            */
            QString time = filemsg->getUploadTime();
            QString filenameStr = filemsg->getFileName();
            int filesize = filemsg->getFileSize();
            qDebug()<< time << " " << filenameStr << " " << filesize << " " << resourceid;
            FileManager *filemag = new FileManager(filenameStr,time,filesize,resourceid);
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(filemag->width(),filemag->height()));
            ui->resourlistWidget->addItem(item);
            ui->resourlistWidget->setItemWidget(item,filemag);
            ui->resourlistWidget->update();
            delete(filemsg);
            filemsg = nullptr;
    }
}
/**
 * @brief BlogFunction::recvServerDownloadFileSignal
 * @param dataval
 * 接收文件服务器的下载资源
 */
void BlogFunction::recvServerDownloadFileSignal(QJsonValue dataval)
{
    CMessageBox::Infomation(this, QStringLiteral("下载资源"),QStringLiteral("下载成功！"));
    qDebug() <<"recvServerwritePicFileSignal(QJsonValue &dataVal)";
    if(dataval.isObject()){
        QJsonObject pic = dataval.toObject();
        int resourceid = pic.value("resourceid").toInt();
        unsigned long filesize = static_cast<unsigned long>(pic.value("filesize").toInt());
        const string filename = pic.value("filename").toString().toStdString();
        const string fromText = pic.value("filetext").toString().toStdString();
        //解码
        unsigned char *decodeText = new unsigned char[filesize];
        CBase64::Decode(fromText,decodeText,&filesize);

        //下载保存文件到本地
        qDebug()<<"filesize = "<<filesize;
        FILE* fp;
        //输入要保存的文件名
         qDebug()<<"Enter the name you wanna to save:";
         //以二进制写入方式
         if ( (fp=fopen(filename.c_str(), "wb"))==NULL)
         {
          qDebug()<<"Open File failed!"<<endl;
         }
         //从buffer中写数据到fp指向的文件中
         fwrite(decodeText,filesize, 1, fp);
         //关闭文件指针
         fclose(fp);
         //释放内存
         delete(decodeText);
         decodeText = nullptr;
    }
}
/**
 * @brief BlogFunction::on_findBlogEditBt_clicked
 * 关键字搜索博客文章
 */
void BlogFunction::on_findBlogEditBt_clicked()
{
    QString keyWork = ui->findkeywordlineEdit->text();
    if(keyWork.count() == 0)
        return ;
    ui->findlistWidget->clear();
    ui->findkeywordlineEdit->setText("");
    //发送博客数据到服务器
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("userid",MyApp::userid);
    data_obj.insert("keyword",keyWork);

    object.insert("data",data_obj);
    object.insert("type",FindBlogMagType::FINDBLOG_MAG);

    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
}

void BlogFunction::on_closeBt_clicked()
{
    int res = CMessageBox::Infomation(this,QStringLiteral("你要关闭吗"), QStringLiteral("关闭UniBlog"));
    if (res == 1){
        emit activeCloseSoftware();
    }
}
void BlogFunction::findblogWidgetAnimation()
{

}
void BlogFunction::on_HelpOurBt_clicked()
{
    m_bIsMenu = !m_bIsMenu;
    SltChangePages(6);
    //m_pEmergancy->show();
    int helpHeight = ui->helpwidget->height();
    int helpWidth = ui->helpwidget->width();
    int aboutHeight = ui->AboutstackedWidget->height();
    int aboutWidth = ui->AboutstackedWidget->width();
    if (m_bIsMenu == true)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(ui->helpwidget, "geometry");
        animation->setDuration(200);
        animation->setStartValue(QRect(0-220, 0, helpWidth, helpHeight));
        animation->setEndValue(QRect(0, 0, helpWidth, helpHeight));

        QPropertyAnimation *aboutAnimation = new QPropertyAnimation(ui->AboutstackedWidget, "geometry");
        aboutAnimation->setDuration(200);
        aboutAnimation->setStartValue(QRect(0, 0, aboutWidth, aboutHeight));
        aboutAnimation->setEndValue(QRect(200, 0, aboutWidth-200, aboutHeight));
        animation->start();
        aboutAnimation->start();
    }
    else
    {
        QPropertyAnimation *animation = new QPropertyAnimation(ui->helpwidget, "geometry");
        animation->setDuration(200);
        animation->setStartValue(QRect(0, 0, helpWidth, helpHeight));
        animation->setEndValue(QRect(0-220, 0, helpWidth, helpHeight));

        //connect(animation, SIGNAL(finished()), this, SLOT(animationFinishedWidgetClose()));
        QPropertyAnimation *aboutAnimation = new QPropertyAnimation(ui->AboutstackedWidget, "geometry");
        aboutAnimation->setDuration(200);
        aboutAnimation->setStartValue(QRect(200, 0, aboutWidth, aboutHeight));
        aboutAnimation->setEndValue(QRect(0, 0, aboutWidth+200, aboutHeight));
        animation->start();
        aboutAnimation->start();
    }
}
/**
 * @brief BlogFunction::on_qssBlackBt_clicked
 * 换肤：黑色炫酷
 */
void BlogFunction::on_qssBlackBt_clicked()
{
    //CommonHelper::setStyle(":/resources/qss/systemQss/black.qss");
    qDebug()<<"黑色换肤";
    QFile qss(":/resources/qss/systemQss/black.qss");
    qss.open(QFile::ReadOnly);
    ui->Blogfunwidget->setStyleSheet(qss.readAll());
    qss.close();
}

void BlogFunction::on_selectPathBt_clicked()
{
    QString filepath = QFileDialog::getOpenFileName();
    ui->FilePathlineEdit->setText(filepath);
}

void BlogFunction::on_sendFileBt_clicked()
{
    QString filePath = ui->FilePathlineEdit->text();
    QFileInfo info(filePath);//文件信息对象

    filemsg = new FileMag(info);
    FTPsocket::getTcpSocket()->sendData(filemsg->getJsonFile());
}

void BlogFunction::on_WidgetMaxBt_clicked()
{
    ui->AboutstackedWidget->showMaximized();
    if(widgetChange)
    this->showFullScreen();
    else
    this->showNormal();
    widgetChange = !widgetChange;
}

void BlogFunction::on_WidgetMinBt_clicked()
{
    this->showMinimized();
}

void BlogFunction::on_userHeadBt_clicked()
{
    UserHeadWidget *userhead = new UserHeadWidget();
    connect(userhead,&UserHeadWidget::replaceHeadSignal,this,&BlogFunction::recvselectReplaceUserHead);
    userhead->show();
}
