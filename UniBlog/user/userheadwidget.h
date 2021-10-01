#ifndef USERHEADWIDGET_H
#define USERHEADWIDGET_H

#include <QWidget>

namespace Ui {
class UserHeadWidget;
}

class UserHeadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserHeadWidget(QWidget *parent = 0);
    ~UserHeadWidget();
    QString selectHead();
    void paintEvent(QPaintEvent *e);
private slots:
    void on_replaceBt_clicked();
    void on_head1_clicked();

    void on_head2_clicked();

    void on_head3_clicked();

    void on_head4_clicked();

    void on_head5_clicked();

    void on_head6_clicked();

    void on_head7_clicked();

    void on_head8_clicked();

signals:
    void replaceHeadSignal(QString headPath);
private:
    Ui::UserHeadWidget *ui;
    int mSelectHead;
};

#endif // USERHEADWIDGET_H
