#ifndef SCPI_H
#define SCPI_H

#include <QTcpSocket>
#include <QThread>
#include "iscpi.h"
#include "scpisession.h"

class ScpiClient final : public IScpiClient, ScpiUpdater
{
public:
    ScpiClient() noexcept;
    ~ScpiClient() noexcept override;

    bool isConnected() const noexcept override;

    QString deviceInfo() const noexcept override;

    QString host() const noexcept override;
    void setHost(const QString &host) noexcept override;

    quint16 port() const noexcept override;
    void setPort(quint16 port) noexcept override;

    qreal minFrequency() const noexcept override;
    void setMinFrequency(qreal minFrequency) noexcept override;

    qreal maxFrequency() const noexcept override;
    void setMaxFrequency(qreal maxFrequency) noexcept override;

    uint bandWidth() const noexcept override;
    void setBandWidth(uint bandWidth) noexcept override;

    uint pointNumber() const noexcept override;
    void setPointNumber(uint pointNumber) noexcept override;

    bool workStatus() const noexcept override;
    void setWorkStatus(bool workStatus) noexcept override;

    void addUpdater(ScpiUpdater *updater) noexcept override;

public slots:
    void connectToHost() noexcept override;
    void disconnectFromHost() noexcept override;

private:
    bool socketConnected() const;
    bool set(const QByteArray &comand);
    std::tuple<QByteArray, bool> get(const QByteArray &comand);

    void prepareDevice();

    void parametersUpdate(const ScpiParameters &parameters) override;
    void dataUpdate(const ScpiData &) override {};

private:
    std::unique_ptr<ScpiSocketSession> _session;
    QThread _sessionThread;
    ScpiParameters _param;
};

#endif // SCPI_H
