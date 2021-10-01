#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "keyboard.h"

class InputMethod : public KeyBoard
{
    Q_OBJECT
public:
    InputMethod();
    ~InputMethod();

    bool eventFilter(QObject *obj, QEvent *event);

public:
    KeyBoard *keyboard;

public:
    void showKeyBoard(int x,int y);
};

#endif // INPUTMETHOD_H
