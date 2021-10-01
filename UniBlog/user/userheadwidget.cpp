#include "userheadwidget.h"
#include "ui_userheadwidget.h"
#include <QFile>
#include <QPainter>
UserHeadWidget::UserHeadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserHeadWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    //这是在Qt的资源下的文件,可以不用在资源下
    QFile file(":/resources/qss/preview.qss");
    //只读方式打开文件
    file.open(QFile::ReadOnly);
    //读取文件的所有内容，并转换成QString类型
    QString styleSheet = tr(file.readAll());
    //当前窗口设置样式表
    this->setStyleSheet(styleSheet);
    file.close();
}
void UserHeadWidget::paintEvent(QPaintEvent *)
 {
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

UserHeadWidget::~UserHeadWidget()
{
    delete ui;
}

QString UserHeadWidget::selectHead()
{
    QString headPath;
    switch(mSelectHead){
        case 0:headPath = ":/resources/image/User/0.bmp";break;
        case 1:headPath = ":/resources/image/User/1.bmp";break;
        case 2:headPath = ":/resources/image/User/2.bmp";break;
        case 3:headPath = ":/resources/image/User/3.bmp";break;
        case 4:headPath = ":/resources/image/User/4.bmp";break;
        case 5:headPath = ":/resources/image/User/5.bmp";break;
        case 6:headPath = ":/resources/image/User/UserHead.jpg";break;
        case 7:headPath = ":/resources/image/User/0.bmp";break;
    }
    return headPath;
}

void UserHeadWidget::on_replaceBt_clicked()
{
    emit replaceHeadSignal(selectHead());
}

void UserHeadWidget::on_head1_clicked()
{
    mSelectHead = 0;
}

void UserHeadWidget::on_head2_clicked()
{
    mSelectHead = 1;
}

void UserHeadWidget::on_head3_clicked()
{
    mSelectHead = 2;
}

void UserHeadWidget::on_head4_clicked()
{
    mSelectHead = 3;
}

void UserHeadWidget::on_head5_clicked()
{
    mSelectHead = 4;
}

void UserHeadWidget::on_head6_clicked()
{
    mSelectHead = 5;
}

void UserHeadWidget::on_head7_clicked()
{
    mSelectHead = 6;
}

void UserHeadWidget::on_head8_clicked()
{
    mSelectHead = 7;
}
