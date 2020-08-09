#ifndef ISCPI_H
#define ISCPI_H

#include <memory>
#include <QObject>
#include <QVector>

struct ScpiUpdater;

class IScpiClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString deviceInfo READ deviceInfo NOTIFY deviceInfoChanged)
    Q_PROPERTY(qreal minFrequency READ minFrequency WRITE setMinFrequency NOTIFY minFrequencyChanged)
    Q_PROPERTY(qreal maxFrequency READ maxFrequency WRITE setMaxFrequency NOTIFY maxFrequencyChanged)
    Q_PROPERTY(uint bandWidth READ bandWidth WRITE setBandWidth NOTIFY bandWidthChanged)
    Q_PROPERTY(uint pointNumber READ pointNumber WRITE setPointNumber NOTIFY pointNumberChanged)
    Q_PROPERTY(bool workStatus READ workStatus WRITE setWorkStatus NOTIFY workStatusChanged)

public:
    static std::unique_ptr<IScpiClient> make() noexcept;
    virtual ~IScpiClient() {};

    virtual bool isConnected() const noexcept = 0;

    virtual QString deviceInfo() const noexcept = 0;

    virtual QString host() const noexcept = 0;
    virtual void setHost(const QString &host) noexcept = 0;

    virtual quint16 port() const noexcept = 0;
    virtual void setPort(quint16 port) noexcept = 0;

    virtual qreal minFrequency() const noexcept = 0;
    virtual void setMinFrequency(qreal minFrequency) noexcept = 0;

    virtual qreal maxFrequency() const noexcept = 0;
    virtual void setMaxFrequency(qreal maxFrequency) noexcept = 0;

    virtual uint bandWidth() const noexcept = 0;
    virtual void setBandWidth(uint bandWidth) noexcept = 0;

    virtual uint pointNumber() const noexcept = 0;
    virtual void setPointNumber(uint pointNumber) noexcept = 0;

    virtual bool workStatus() const noexcept = 0;
    virtual void setWorkStatus(bool workStatus) noexcept = 0;

    virtual void addUpdater(ScpiUpdater *updater) noexcept = 0;

public slots:
    virtual void connectToHost() noexcept = 0;
    virtual void disconnectFromHost() noexcept = 0;

signals:
    void errorOccured(QString error);
    void hostChanged(QString host);
    void portChanged(quint16 port);
    void minFrequencyChanged(qreal minFrequency);
    void maxFrequencyChanged(qreal maxFrequency);
    void bandWidthChanged(uint bandWidth);
    void pointNumberChanged(uint pointNumber);
    void deviceInfoChanged(QString deviceInfo);
    void connectedChanged(bool isConnected);
    void workStatusChanged(bool workStatus);
};

struct ScpiParameters
{
    QString info;
    qreal minFrequency {1};
    qreal maxFrequency {9};
    uint bandWidth {1};
    uint pointNumber {1};
};

struct ScpiData
{
    QVector<qreal> frequency;
    QVector<qreal> value;
};

struct ScpiUpdater
{
    virtual void parametersUpdate(const ScpiParameters &parameters) = 0;
    virtual void dataUpdate(const ScpiData &data) = 0;
};

#endif // ISCPI_H
