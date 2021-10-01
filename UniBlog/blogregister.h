#ifndef BLOGREGISTER_H
#define BLOGREGISTER_H

#include <QWidget>
#include "basewidget/mywidget.h"
namespace Ui {
class BlogRegister;
}

class BlogRegister : public MyWidget
{
    Q_OBJECT

public:
    explicit BlogRegister(QWidget *parent = 0);
    ~BlogRegister();
    void readLoginWidgetQss();
    void paintEvent(QPaintEvent *e);
    void setStyle();
private slots:
    void on_loginBt_clicked();

    void registerMag(bool state);
    void on_registerBt_clicked();
    void on_closeBt_clicked();

signals:
    void backLoginSignal();
private:
    Ui::BlogRegister *ui;
};

#endif // BLOGREGISTER_H
