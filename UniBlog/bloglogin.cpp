#include <QGraphicsDropShadowEffect>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QPropertyAnimation>
#include "bloglogin.h"
#include "ui_bloglogin.h"
#include "myapp.h"
#include "blogfunction.h"
#include "blogregister.h"
#include "basewidget/customwidget.h"
#include "File/FTPsocket.h"
BlogLogin::BlogLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlogLogin)
{
    qDebug()<<"main thread id = " << QThread::currentThreadId();
    ui->setupUi(this);
    animation = nullptr;
    blogfun = nullptr;
    loading = nullptr;
    im = new KeyBoard;
    ui->PasswordlineEdit->installEventFilter(this);
    ui->UsernamelineEdit->installEventFilter(this);
    MySocket::getTcpSocket()->initMySocket();
    FTPsocket::getTcpSocket()->initFTPsocket();
    //设置登录界面的样式属性
    setBlogLoginWidget();
    //读取样式表qss
    readLoginWidgetQss();
    //网络信号接口
    netWorkSingal();
}
void BlogLogin::netWorkSingal(){
    connect(MySocket::getTcpSocket(),SIGNAL(connectTCPServerSignalToUi(bool)),this,SLOT(connectTCPServerSignal(bool)));
    connect(FTPsocket::getTcpSocket(),SIGNAL(connectFTPServerSignalToUi(bool)),this,SLOT(connectFTPServerSignal(bool)));
    //登录成功信号
    connect(MySocket::getTcpSocket(),SIGNAL(loginMagSignal(bool)),this,SLOT(loginMag(bool)));
}
/**
 * @brief BlogLogin::loadingUserAllBlog
 */
void BlogLogin::loadingUserAllBlog()
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

BlogLogin::~BlogLogin()
{
    qDebug()<<"~BlogLogin()";
    if(animation != nullptr){
        animation->stop();
        animation->deleteLater();
        animation = nullptr;
        qDebug()<<"~animation";
    }
    if(blogfun != nullptr){
        blogfun->deleteLater();
        blogfun = nullptr;
        qDebug()<<"~blogfun";
    }
    if(loading != nullptr){
        loading->deleteLater();
        loading = nullptr;
        qDebug()<<"~loading";
    }
    im->deleteLater();
    im = nullptr;
    delete ui;
}
/**
 * @brief BlogLogin::setBlogLoginWidget
 */
void BlogLogin::setBlogLoginWidget()
{
    QAction *UserAction = new QAction(this);
    UserAction->setIcon(QIcon(":/resources/image/keypad/keypad3.png"));
    ui->UsernamelineEdit->addAction(UserAction,QLineEdit::TrailingPosition);
    connect(UserAction, SIGNAL(triggered(bool)), this, SLOT(userOnSearch(bool)));

    QAction *UserIcoAction = new QAction(this);
    UserIcoAction->setIcon(QIcon(":/resources/image/login/ic_user.png"));
    ui->UsernamelineEdit->addAction(UserIcoAction,QLineEdit::LeadingPosition);


    QAction *PassAction = new QAction(this);
    PassAction->setIcon(QIcon(":/resources/image/keypad/keypad3.png"));
    ui->PasswordlineEdit->addAction(PassAction,QLineEdit::TrailingPosition);
    connect(PassAction, SIGNAL(triggered(bool)), this, SLOT(PassOnSearch(bool)));

    QAction *PassIcoAction = new QAction(this);
    PassIcoAction->setIcon(QIcon(":/resources/image/login/ic_lock.png"));
    ui->PasswordlineEdit->addAction(PassIcoAction,QLineEdit::LeadingPosition);
    //QLineEdit::LeadingPosition 在左侧
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
    ui->Loginwidget->setGraphicsEffect(shadow);
    //给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
    ui->verticalLayout->setMargin(10);
}
/**
 * @brief BlogLogin::readLoginWidgetQss
 */
void BlogLogin::readLoginWidgetQss()
{
    //这是在Qt的资源下的文件,可以不用在资源下
    QFile file(":/resources/qss/login.qss");
    //只读方式打开文件
    file.open(QFile::ReadOnly);
    //读取文件的所有内容，并转换成QString类型
    QString styleSheet = tr(file.readAll());
    //当前窗口设置样式表
    ui->Loginwidget->setStyleSheet(styleSheet);
    file.close();
}
/**
 * @brief BlogLogin::mouseMoveEvent
 * @param e
 */
void BlogLogin::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}
/**
 * @brief BlogLogin::mousePressEvent
 * @param e
 */
void BlogLogin::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}
/**
 * @brief BlogLogin::mouseReleaseEvent
 */
void BlogLogin::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}
/**
 * @brief BlogLogin::on_closeBt_clicked
 */
void BlogLogin::on_closeBt_clicked()
{
    qDebug()<<"qApp->quit";
    qApp->quit();
}
/**
 * @brief BlogLogin::on_settingBt_clicked
 */
void BlogLogin::on_settingBt_clicked()
{
    ui->Loginwidget->nextPage();
}
/**
 * @brief BlogLogin::on_SettingcloseBt_clicked
 */
void BlogLogin::on_SettingcloseBt_clicked()
{
    ui->Loginwidget->nextPage();
}
/**
 * @brief BlogLogin::on_loginBt_clicked
 */
void BlogLogin::on_loginBt_clicked()
{
    if(ui->UsernamelineEdit->text().isEmpty() || ui->PasswordlineEdit->text().isEmpty()){
        CMessageBox::Infomation(this, QStringLiteral("登录提示 "), QStringLiteral("请输入登录的账号和密码 "));
        return ;
    }
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("username",ui->UsernamelineEdit->text());
    data_obj.insert("password",ui->PasswordlineEdit->text());

    object.insert("data",data_obj);
    object.insert("type",LoginMagType::LOGIN_MAG);


    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据 ")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
}
/**
 * @brief BlogLogin::on_registerBt_clicked
 */
void BlogLogin::on_registerBt_clicked()
{
    BlogRegister *m_reg = new BlogRegister();
    m_reg->show();
    this->hide();
    connect(m_reg,&BlogRegister::backLoginSignal,this,[=](){
        m_reg->close();
        this->show();
    });
}
/**
 * @brief BlogLogin::connectServerSignal
 * @param state
 * 是否连接消息服务器，对Lable重写状态
 */
void BlogLogin::connectTCPServerSignal(bool state)
{
    if(state){
        ui->ServerStateLabel->setText(QStringLiteral("已连接消息服务器"));
    }else{
        ui->ServerStateLabel->setText(QStringLiteral("未连接消息服务器"));
    }
}
/**
 * @brief BlogLogin::connectFTPServerSignal
 * @param state
 * 是否连接文件服务器，对Lable重写状态
 */
void BlogLogin::connectFTPServerSignal(bool state)
{
    if(state){
        ui->FTPServerStateLabel->setText(QStringLiteral("已连接文件服务器"));
    }else{
        ui->FTPServerStateLabel->setText(QStringLiteral("未连接文件服务器"));
    }
}
/**
 * @brief BlogLogin::userOnSearch
 * @param tiggflag
 * 账号输入框的键盘触发
 */
void BlogLogin::userOnSearch(bool tiggflag)
{
    im->show();
    im->setMovePos(this->x()+200,this->y()+350);
    disconnect(im,SIGNAL(setvalue(QString)),0,0);
    connect(im,SIGNAL(setvalue(QString)),ui->UsernamelineEdit,SLOT(setText(QString)));
}
/**
 * @brief BlogLogin::PassOnSearch
 * @param tiggflag
 * 密码输入框的键盘触发
 */
void BlogLogin::PassOnSearch(bool tiggflag)
{
    im->show();
    im->setMovePos(this->x()+200,this->y()+350);
    disconnect(im,SIGNAL(setvalue(QString)),0,0);
    connect(im,SIGNAL(setvalue(QString)),ui->PasswordlineEdit,SLOT(setText(QString)));
}

void BlogLogin::loginMag(bool state)
{
    if(state){
        //清空输入框
        ui->UsernamelineEdit->setText("");
        ui->PasswordlineEdit->setText("");
        //加载动画
        loading = new LoadingDialog(this);
        loading->moveToCenter(this);
        loading->show();
        //主界面
        blogfun = new BlogFunction();
        //加载该用户的所有博客
        //loadingUserAllBlog();
        connect(blogfun,&BlogFunction::CloseblogfunWidgetSingal,this,[=](){
            blogfun->close();
            blogfun->deleteLater();
            blogfun = nullptr;
            qDebug() << "~blogfun";
            loading->deleteLater();
            loading = nullptr;
            qDebug() << "~loading";
            this->show();
            animation->start();
        },Qt::UniqueConnection);
        connect(blogfun,SIGNAL(activeCloseSoftware()),this,SLOT(on_closeBt_clicked()));
        //启动界面关闭特效动画
        controlWidgetShow();
    }else{
        CMessageBox::Infomation(this, QStringLiteral("登录提示"), QStringLiteral("登录失败，输入用户或密码错误"));
    }
}

//============================================animation=============================================
/**
 * @brief BlogLogin::close
 * 动画运行结束，调用close()槽函数
 */
void BlogLogin::animationEndWidgetClose()
{
    if(animation->direction() == QAbstractAnimation::Backward)
    {
        this->hide();
        blogfun->show();
    }
}
/**
 * @brief BlogLogin::on_animation_stateChanged
 * @param newState
 * @param oldState
 * 动画运行状态
 */
void BlogLogin::on_animation_stateChanged(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    if(newState != QAbstractAnimation::Running) {
            if (animation->direction() == QAbstractAnimation::Forward) {
                animation->setDirection(QAbstractAnimation::Backward);
            } else {
                animation->setDirection(QAbstractAnimation::Forward);
            }
    }
}

/**
 * @brief BlogLogin::controlHides
 * 界面关闭动画设置:状态Forward(逐渐消失)和Backward(逐渐回显)
 */
void BlogLogin::controlWidgetShow()
{
    animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(2000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(animationEndWidgetClose()));
    connect(animation, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
                this, SLOT(on_animation_stateChanged(QAbstractAnimation::State,QAbstractAnimation::State))
                );
}
