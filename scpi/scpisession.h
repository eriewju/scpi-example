#ifndef SCPISESSION_H
#define SCPISESSION_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include "iscpi.h"

class ScpiSocketSession : public QObject, public ScpiUpdater
{
    Q_OBJECT

public:
    ScpiSocketSession();
    ~ScpiSocketSession();

    bool isConnected() const;

    QString host() const;
    void setHost(const QString &host);

    quint16 port() const;
    void setPort(quint16 port);

    uint timeoutMs() const;
    void setTimeoutMs(uint timeout);

    void start();
    void stop();
    bool isWork();

    ScpiParameters parameters() const;
    void setMinFrequency(qreal minFrequency);
    void setMaxFrequency(qreal maxFrequency);
    void setBandWidth(uint bandWidth);
    void setPointNumber(uint pointNumber);

    void connectToHost();
    void disconnectFromHost();

    void addUpdater(ScpiUpdater *updater);

signals:
    void connectedToHost();
    void disconnectedFromHost();
    void lostConnection();

private:
    void push(std::function<void()> &&task);
    void invokeFront();
    void handleTimeout() {}; // TODO: implement it
    void reset();

    void getParameters();
    void prepareDevice();
    void sendDataComand();

    void parametersUpdate(const ScpiParameters &parameters) override;
    void dataUpdate(const ScpiData &data) override;

private:
    QString _host {"127.0.0.1"};
    quint16 _port {5025};
    uint _timeout {300};
    QTcpSocket _socket;
    ScpiParameters _params;
    QList<ScpiUpdater*> _updaters;
    bool _analyzing {false};

    QMutex _mutex{QMutex::NonRecursive};
    QList<std::function<void()>> _tasks;
};

#endif // SCPISESSION_H
