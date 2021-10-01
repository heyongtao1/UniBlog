#ifndef PREVIEWBLOGWIDGET_H
#define PREVIEWBLOGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "mywidget.h"
namespace Ui {
class PreviewBlogWidget;
}

class PreviewBlogWidget : public MyWidget
{
    Q_OBJECT

public:
    PreviewBlogWidget();
    ~PreviewBlogWidget();
    void readLoginWidgetQss();
    void setAllUiWidget(QString &headPath,QString,QString,QString,QString);
    void setUserHead(QString &headPath);
    void setUsername(QString _username);
    void setText(QString _text);
    void setTextHead(QString _texthead);
    void settime(QString _time);
    QPushButton* getPreviewBt();
private slots:


private:
    Ui::PreviewBlogWidget *ui;
};

#endif // PREVIEWBLOGWIDGET_H
