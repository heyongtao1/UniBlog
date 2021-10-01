#ifndef COMMENTWIDGET_H
#define COMMENTWIDGET_H

#include <QWidget>

namespace Ui {
class CommentWidget;
}

class CommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentWidget(QWidget *parent = 0);
    ~CommentWidget();

private:
    Ui::CommentWidget *ui;
};

#endif // COMMENTWIDGET_H
