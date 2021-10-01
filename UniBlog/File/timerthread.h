#ifndef TIMERTHREAD_H
#define TIMERTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
class TimerThread : public QThread
{
    Q_OBJECT
public:
    explicit TimerThread();
    ~TimerThread();
    void run();
signals:

public slots:
    void timeoutSlot();
private:
    QTimer *m_pTimer;
};

#endif // TIMERTHREAD_H
