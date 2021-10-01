#include <QFile>
#include <QTextCursor>
#include "previewblogwidget.h"
#include "ui_previewblogwidget.h"
/**
 * @brief PreviewBlogWidget::PreviewBlogWidget
 * @param parent
 * 新建博客文本类
 */
PreviewBlogWidget::PreviewBlogWidget() :
    MyWidget(),
    ui(new Ui::PreviewBlogWidget)
{
    ui->setupUi(this);
    readLoginWidgetQss();
    ui->text->setStyleSheet(QString("QTextEdit{background-color: transparent;}"));
}

PreviewBlogWidget::~PreviewBlogWidget()
{
    delete ui;
}

void PreviewBlogWidget::readLoginWidgetQss()
{
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

void PreviewBlogWidget::setAllUiWidget(QString &headPath,QString _username,QString _time,QString _texthead,QString _text)
{
    setUserHead(headPath);
    setUsername(_username);
    setText(_text);
    setTextHead(_texthead);
    settime(_time);
}

void PreviewBlogWidget::setUserHead(QString &headPath)
{
    ui->userhead->setPixmap(QPixmap(headPath));
}

void PreviewBlogWidget::setUsername(QString _username)
{
    ui->usernamelable->setText(_username);
}
/**
 * @brief PreviewBlogWidget::setText
 * @param _text
 * 未设置根据文本长度自动增长QTextEdit高度
 */
void PreviewBlogWidget::setText(QString _text)
{
    ui->text->setText(_text);
    //设置禁止编辑
    ui->text->setReadOnly(true);
    //设置QTextEdit的显示最大段数5
    //ui->text->document()->setMaximumBlockCount(5);
    //设置QTextEdit的滚动条为隐藏
    ui->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void PreviewBlogWidget::setTextHead(QString _texthead)
{
    //设置标题Label适应文本长度，自动换行
    ui->headtext->adjustSize();
    ui->headtext->setWordWrap(true);
    ui->headtext->setText(_texthead);
}

void PreviewBlogWidget::settime(QString _time)
{
    ui->timelable->setText(_time);
}

QPushButton *PreviewBlogWidget::getPreviewBt()
{
    return ui->previewBt;
}
