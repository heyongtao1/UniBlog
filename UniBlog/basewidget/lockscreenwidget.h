#ifndef LOCKSCREENWIDGET_H
#define LOCKSCREENWIDGET_H

#include <QWidget>
#include "mywidget.h"
namespace Ui {
class LockScreenWidget;
}

class LockScreenWidget : public MyWidget
{
    Q_OBJECT

public:
    explicit LockScreenWidget(QWidget *parent = 0);
    ~LockScreenWidget();
    bool m_unlockFlag;
    void setBloglockWidget();
    void readlockWidgetQss();
private slots:
    void on_okBt_clicked();
private:
    Ui::LockScreenWidget *ui;
};

#endif // LOCKSCREENWIDGET_H
