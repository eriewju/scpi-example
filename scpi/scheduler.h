#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QTcpSocket>

class Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler();

private:
    QTcpSocket socket;
};

#endif // SCHEDULER_H
