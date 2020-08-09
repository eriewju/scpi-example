#include "scpisession.h"
#include <QMutexLocker>
#include <QTimer>
#include <QEventLoop>

ScpiSocketSession::ScpiSocketSession()
    : _socket(this)
{
    connect(&_socket, &QAbstractSocket::stateChanged, this,
            [this](QAbstractSocket::SocketState s) {
        if (s == QTcpSocket::ConnectedState) {
            emit connectedToHost();
            getParameters(); // get actual parameters
        }
        else {
            emit disconnectedFromHost();
            reset();
        }
    });
}

ScpiSocketSession::~ScpiSocketSession()
{
    _socket.disconnectFromHost();
}

bool ScpiSocketSession::isConnected() const
{
    return _socket.state() == QTcpSocket::ConnectedState;
}

QString ScpiSocketSession::host() const
{
    return _host;
}

void ScpiSocketSession::setHost(const QString &host)
{
    _host = host;
}

quint16 ScpiSocketSession::port() const
{
    return _port;
}

void ScpiSocketSession::setPort(quint16 port)
{
    _port = port;
}

uint ScpiSocketSession::timeoutMs() const
{
    return _timeout;
}

void ScpiSocketSession::setTimeoutMs(uint timeout)
{
    _timeout = timeout;
}

void ScpiSocketSession::start()
{
    _analyzing = true;
    QMetaObject::invokeMethod(this, [this] {
        prepareDevice();
        sendDataComand();
    });
}

void ScpiSocketSession::stop()
{
    _analyzing = false;
    QMetaObject::invokeMethod(this, [this] {
        dataUpdate({});
    });
}

bool ScpiSocketSession::isWork()
{
    return _analyzing;
}

ScpiParameters ScpiSocketSession::parameters() const
{
    return _params;
}

void ScpiSocketSession::setMinFrequency(qreal minFrequency)
{
    // чтобы пуш был в потоке сессии
    QMetaObject::invokeMethod(this, [this, minFrequency] {
        push([this, minFrequency] {
            auto comand = QString("SENS:FREQ:STAR %1GHZ\n")
                    .arg(minFrequency)
                    .toUtf8();
            _socket.write(comand);
            if (_socket.waitForBytesWritten(_timeout)) {
                _params.minFrequency = minFrequency;
                parametersUpdate(_params);
            }
            else {
                handleTimeout();
            }
        });
    });
}

void ScpiSocketSession::setMaxFrequency(qreal maxFrequency)
{
    QMetaObject::invokeMethod(this, [this, maxFrequency] {
        push([this, maxFrequency] {
            auto comand = QString("SENS:FREQ:STOP %1GHZ\n")
                    .arg(maxFrequency)
                    .toUtf8();
            _socket.write(comand);
            if (_socket.waitForBytesWritten(_timeout)) {
                _params.maxFrequency = maxFrequency;
                parametersUpdate(_params);
            }
            else {
                handleTimeout();
            }
        });
    });
}

void ScpiSocketSession::setBandWidth(uint bandWidth)
{
    QMetaObject::invokeMethod(this, [this, bandWidth] {
        push([this, bandWidth] {
            auto comand = QString("SENS:BWID %1\n")
                    .arg(bandWidth)
                    .toUtf8();
            _socket.write(comand);
            if (_socket.waitForBytesWritten(_timeout)) {
                _params.bandWidth = bandWidth;
                parametersUpdate(_params);
            }
            else {
                handleTimeout();
            }
        });
    });
}

void ScpiSocketSession::setPointNumber(uint pointNumber)
{
    QMetaObject::invokeMethod(this, [this, pointNumber] {
        push([this, pointNumber] {
            auto comand = QString("SENS:SWE:POIN %1\n")
                    .arg(pointNumber)
                    .toUtf8();
            _socket.write(comand);
            if (_socket.waitForBytesWritten(_timeout)) {
                _params.pointNumber = pointNumber;
                parametersUpdate(_params);
            }
            else {
                handleTimeout();
            }
        });
    });
}

void ScpiSocketSession::connectToHost()
{
    QMetaObject::invokeMethod(this, [this] {
        push([this] {
            _socket.connectToHost(_host, _port);
            if (!_socket.waitForConnected(_timeout))
                handleTimeout();
        });
    });
}

void ScpiSocketSession::disconnectFromHost()
{
    QMetaObject::invokeMethod(this, [this] {
        stop();
        _socket.abort();
        _socket.disconnectFromHost();
        parametersUpdate({});
        dataUpdate({});

        QMutexLocker lock(&_mutex);
        Q_UNUSED(lock);
        _tasks.clear();
    });
}

void ScpiSocketSession::addUpdater(ScpiUpdater *updater)
{
    _updaters.append(updater);
}

void ScpiSocketSession::push(std::function<void ()> &&task)
{
    {
        QMutexLocker lock(&_mutex);
        Q_UNUSED(lock);
        _tasks.push_back(std::move(task));
    }

    if (_tasks.size() == 1)
        invokeFront();
}

void ScpiSocketSession::invokeFront()
{
    if (_tasks.empty())
        return;

    // invoke and drop
    _tasks.front()(); {
        QMutexLocker lock(&_mutex);
        Q_UNUSED(lock);
        _tasks.pop_front();
    }

    QMetaObject::invokeMethod(this, &ScpiSocketSession::invokeFront,
                              Qt::QueuedConnection);
}

void ScpiSocketSession::reset()
{
    _analyzing = false;
    QMetaObject::invokeMethod(this, [this] {
        parametersUpdate({});
        dataUpdate({});
    });
}

void ScpiSocketSession::getParameters()
{
    push([this] {
        _socket.write("*IDN?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        _params.info = _socket.readLine();

        _socket.write("SENS:FREQ:STAR?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        _params.minFrequency = _socket.readLine().toDouble() / 1e9;

        _socket.write("SENS:FREQ:STOP?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        _params.maxFrequency = _socket.readLine().toDouble() / 1e9;

        _socket.write("SENS:BWID?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        _params.bandWidth = _socket.readLine().toUInt();

        _socket.write("SENS:SWE:POIN?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        _params.pointNumber = _socket.readLine().toUInt();

        parametersUpdate(_params);
    });
}

void ScpiSocketSession::prepareDevice()
{
    push([this] {
        _socket.write("CALC:PAR:DEF S11\n");
        _socket.write("CALC:PAR:SEL\n");
        _socket.write("CALC:FORM MLOG\n");
        if (!_socket.waitForBytesWritten(_timeout))
            handleTimeout();
    });
}

void ScpiSocketSession::sendDataComand()
{
    push([this] {
        _socket.write(":TRIG:SOUR BUS\n");
        _socket.write(":TRIG:SING\n");
        _socket.write("*OPC?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        auto response = _socket.readLine();
        if (response.front() != '1') {
            qWarning() << "OPC Response is not 1";
            return;
        }

        _socket.write("CALC:DATA:FDAT?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        auto data = _socket.readLine().split(',');

        _socket.write("SENS:FREQ:DATA?\n");
        if (!_socket.waitForReadyRead(_timeout)) {
            handleTimeout();
            return;
        }
        auto freq = _socket.readLine().split(',');

        ScpiData sd;
        auto length = freq.length();
        for (int i = 0; i < length; i++) {
            sd.value.push_back(data[2 * i].toDouble());
            sd.frequency.push_back(freq[i].toDouble() / 1e9); // NOTE: GHZ
        }

        dataUpdate(std::move(sd));
        if (_analyzing)
            sendDataComand();
    });
}

void ScpiSocketSession::parametersUpdate(const ScpiParameters &parameters)
{
    for (auto updater : _updaters)
        updater->parametersUpdate(parameters);
}

void ScpiSocketSession::dataUpdate(const ScpiData &data)
{
    for (auto updater : _updaters)
        updater->dataUpdate(data);
}
