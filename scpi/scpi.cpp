#include "scpi.h"

std::unique_ptr<IScpiClient> IScpiClient::make() noexcept
{
    return std::make_unique<ScpiClient>();
}

ScpiClient::ScpiClient() noexcept
    : _session(std::make_unique<ScpiSocketSession>())
{
    connect(_session.get(), &ScpiSocketSession::connectedToHost, this, [this] {
        emit connectedChanged(true);
    });
    connect(_session.get(), &ScpiSocketSession::lostConnection, this, [this] {
        // TODO: handle errorOccured
        emit connectedChanged(false);
    });
    connect(_session.get(), &ScpiSocketSession::disconnectedFromHost, this, [this] {
        emit connectedChanged(false);
        emit workStatusChanged(false);
    });

    _session->moveToThread(&_sessionThread);
    _sessionThread.start();

    _session->setTimeoutMs(3000);
    _session->addUpdater(this);
}

ScpiClient::~ScpiClient() noexcept
{
    _session->stop();
    _sessionThread.quit();
}

bool ScpiClient::isConnected() const noexcept
{
    return socketConnected();
}

void ScpiClient::connectToHost() noexcept
{
    _session->connectToHost();
}

void ScpiClient::disconnectFromHost() noexcept
{
    _session->disconnectFromHost();
}

QString ScpiClient::deviceInfo() const noexcept
{
    return _param.info;
}

QString ScpiClient::host() const noexcept
{
    return _session->host();
}

void ScpiClient::setHost(const QString &host) noexcept
{
    if (isConnected())
        return;
    _session->setHost(host);
    emit hostChanged(host);
}

quint16 ScpiClient::port() const noexcept
{
    return _session->port();
}

void ScpiClient::setPort(quint16 port) noexcept
{
    if (isConnected())
        return;
    _session->setPort(port);
    emit portChanged(port);
}

qreal ScpiClient::minFrequency() const noexcept
{
    return _param.minFrequency;
}

void ScpiClient::setMinFrequency(qreal minFrequency) noexcept
{
    _session->setMinFrequency(minFrequency);
}

qreal ScpiClient::maxFrequency() const noexcept
{
    return _param.maxFrequency;
}

void ScpiClient::setMaxFrequency(qreal maxFrequency) noexcept
{
    _session->setMaxFrequency(maxFrequency);
}

uint ScpiClient::bandWidth() const noexcept
{
    return _param.bandWidth;
}

void ScpiClient::setBandWidth(uint bandWidth) noexcept
{
    _session->setBandWidth(bandWidth);
}

uint ScpiClient::pointNumber() const noexcept
{
    return _param.pointNumber;
}

void ScpiClient::setPointNumber(uint pointNumber) noexcept
{
    _session->setPointNumber(pointNumber);
}

bool ScpiClient::workStatus() const noexcept
{
    return _session->isWork();
}

void ScpiClient::setWorkStatus(bool workStatus) noexcept
{
    if (workStatus)
        _session->start();
    else
        _session->stop();
    emit workStatusChanged(_session->isWork());
}

void ScpiClient::addUpdater(ScpiUpdater *updater) noexcept
{
    _session->addUpdater(updater);
}

bool ScpiClient::socketConnected() const
{
    return _session->isConnected();
}

void ScpiClient::parametersUpdate(const ScpiParameters &param)
{
    auto check = [=](auto& newVal, auto& val, auto valChanged) {
        if (val != newVal) {
            val = newVal;
            (this->*valChanged)(val);
        }
    };

    check(param.info, _param.info, &IScpiClient::deviceInfoChanged);
    check(param.bandWidth, _param.bandWidth, &IScpiClient::bandWidthChanged);
    check(param.pointNumber, _param.pointNumber, &IScpiClient::pointNumberChanged);
    check(param.minFrequency, _param.minFrequency, &IScpiClient::minFrequencyChanged);
    check(param.maxFrequency, _param.maxFrequency, &IScpiClient::maxFrequencyChanged);
    _param = param;
}
