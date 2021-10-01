#include "blogregister.h"
#include "ui_blogregister.h"

#include <QFile>
#include <QJsonObject>
#include <QPainter>
#include "mysocket.h"
#include "basewidget/customwidget.h"
BlogRegister::BlogRegister(QWidget *parent) :
    MyWidget(parent),
    ui(new Ui::BlogRegister)
{
    ui->setupUi(this);
    readLoginWidgetQss();
    setStyle();
    //设置无边框
    setAttribute(Qt::WA_StyledBackground);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    connect(MySocket::getTcpSocket(),SIGNAL(registerMagSignal(bool)),this,SLOT(registerMag(bool)));

}
void BlogRegister::readLoginWidgetQss()
{
    //这是在Qt的资源下的文件,可以不用在资源下
    QFile file(":/resources/qss/register.qss");
    //只读方式打开文件
    file.open(QFile::ReadOnly);
    //读取文件的所有内容，并转换成QString类型
    QString styleSheet = tr(file.readAll());
    //当前窗口设置样式表
    this->setStyleSheet(styleSheet);
    file.close();
}
void BlogRegister::registerMag(bool state)
{
    if(state){
        CMessageBox::Infomation(this, QStringLiteral("注册提示"), QStringLiteral("注册成功，请登录使用Uniblog"));
    }else{
        CMessageBox::Infomation(this, QStringLiteral("注册提示 "), QStringLiteral("注册失败，该用户已存在 "));
    }
}
void BlogRegister::paintEvent(QPaintEvent *event)
{

}

void BlogRegister::setStyle()
{
    QAction *UserAction = new QAction(this);
    UserAction->setIcon(QIcon(":/resources/image/register/xing.png"));
    ui->UsernamelineEdit->addAction(UserAction,QLineEdit::TrailingPosition);

    QAction *cPassAction = new QAction(this);
    cPassAction->setIcon(QIcon(":/resources/image/register/xing.png"));
    ui->CfPasswordlineEdit->addAction(cPassAction,QLineEdit::TrailingPosition);


    QAction *PassAction = new QAction(this);
    PassAction->setIcon(QIcon(":/resources/image/register/xing.png"));
    ui->PasswordlineEdit->addAction(PassAction,QLineEdit::TrailingPosition);
}
    BlogRegister::~BlogRegister()
{
    delete ui;
}

void BlogRegister::on_loginBt_clicked()
{
    emit backLoginSignal();
    disconnect(MySocket::getTcpSocket(),SIGNAL(registerMagSignal(bool)),0,0);
}

void BlogRegister::on_registerBt_clicked()
{
    if(ui->UsernamelineEdit->text().isEmpty() || ui->PasswordlineEdit->text().isEmpty()){
        CMessageBox::Infomation(this, QStringLiteral("注册提示 "), QStringLiteral("请输入注册的账号和密码 "));
        return ;
    }
    if(ui->PasswordlineEdit->text() != ui->CfPasswordlineEdit->text()){
        CMessageBox::Infomation(this, QStringLiteral("注册提示 "), QStringLiteral("请输入注册密码不一致 "));
        return ;
    }
    QJsonObject object;
    QJsonObject data_obj;
    data_obj.insert("username",ui->UsernamelineEdit->text());
    data_obj.insert("password",ui->PasswordlineEdit->text());

    object.insert("data",data_obj);
    object.insert("type",RegistMagType::REGIST_MAG);


    QJsonDocument document;
    document.setObject(object);

    qDebug()<<QStringLiteral("发送内容数据 ")<<document.toJson();
    MySocket::getTcpSocket()->sendData(document);
}

void BlogRegister::on_closeBt_clicked()
{
    emit backLoginSignal();
    disconnect(MySocket::getTcpSocket(),SIGNAL(registerMagSignal(bool)),0,0);
}
