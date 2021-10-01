#include <QGraphicsDropShadowEffect>
#include "lockscreenwidget.h"
#include "ui_lockscreenwidget.h"
/**
 * @brief LockScreenWidget::LockScreenWidget
 * @param parent
 * 锁屏窗口类
 */
LockScreenWidget::LockScreenWidget(QWidget *parent) :
    MyWidget(parent),
    ui(new Ui::LockScreenWidget)
{
    ui->setupUi(this);
    setBloglockWidget();
    readlockWidgetQss();
}
void LockScreenWidget::setBloglockWidget()
{
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
    ui->lockwidget->setGraphicsEffect(shadow);
    //给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
    ui->verticalLayout->setMargin(10);
}
void LockScreenWidget::readlockWidgetQss()
{
    //这是在Qt的资源下的文件,可以不用在资源下
    QFile file(":/resources/qss/blog.qss");
    //只读方式打开文件
    file.open(QFile::ReadOnly);
    //读取文件的所有内容，并转换成QString类型
    QString styleSheet = tr(file.readAll());
    //当前窗口设置样式表
    ui->lockwidget->setStyleSheet(styleSheet);
    file.close();
}
LockScreenWidget::~LockScreenWidget()
{
    delete ui;
}

void LockScreenWidget::on_okBt_clicked()
{
    m_unlockFlag = true;
    this->close();
    //解除阻塞其他窗口
    //this->setWindowModality(Qt::WindowModal);
}
