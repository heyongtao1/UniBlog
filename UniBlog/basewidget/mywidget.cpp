#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{

}
/**
 * @brief MyWidget::mouseMoveEvent
 * @param e
 */
void MyWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}
/**
 * @brief MyWidget::mousePressEvent
 * @param e
 */
void MyWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}
/**
 * @brief MyWidget::mouseReleaseEvent
 */
void MyWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_mousePressed = false;
}
