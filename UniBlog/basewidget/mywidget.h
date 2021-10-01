#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QMouseEvent>
#include <QWidget>

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = nullptr);
protected:
    QPoint mousePoint;
    bool m_mousePressed;
    void mouseMoveEvent(QMouseEvent *e);//鼠标移动事件
    void mousePressEvent(QMouseEvent *e);//鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *);//鼠标松开事件
signals:

public slots:
};

#endif // MYWIDGET_H
