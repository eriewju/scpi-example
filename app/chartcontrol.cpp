#include "chartcontrol.h"
#include <QDebug>
#include <QAbstractSeries>
#include <QLineSeries>
#include <QValueAxis>

using namespace QtCharts;

ChartControl::ChartControl(QObject *chartObject, QObject *parent)
    : QObject(parent)
    , _chart(chartObject)
{
    QAbstractSeries *series {nullptr};
    bool f = QMetaObject::invokeMethod(_chart, "series",
            Q_RETURN_ARG(QAbstractSeries*, series),
            Q_ARG(QString, "lineSeries"));
    Q_ASSERT(f && series);
    _series = qobject_cast<QLineSeries*>(series);

    for (auto axis : _series->attachedAxes()) {
        axis->metaObject()->className();
        if ((axis->alignment() & Qt::AlignBottom) != 0)
            _axisX = qobject_cast<QValueAxis*>(axis);
        if ((axis->alignment() & Qt::AlignLeft) != 0)
            _axisY = qobject_cast<QValueAxis*>(axis);
    }

    _axisY->setTitleText("S11");
    _axisY->setRange(-50, 50);
    _axisY->setTickCount(11);

    _axisX->setTitleText("Frequency");
    _axisX->setRange(1, 9);
    _axisX->setTickCount(9);
}

void ChartControl::parametersUpdate(const ScpiParameters &parameters)
{
    qreal min = parameters.minFrequency;
    qreal max = parameters.maxFrequency;
    _axisX->setRange(min, max);
    _axisX->setTickCount(max - min + 1);
}

void ChartControl::dataUpdate(const ScpiData &data)
{
    QVector<QPointF> points;
    for (int i = 0; i < data.frequency.length(); i++) {
        points.push_back({
            data.frequency[i],
            data.value[i]
        });
    }
    _series->replace(points);
}
