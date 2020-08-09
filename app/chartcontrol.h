#ifndef CHARTCONTROL_H
#define CHARTCONTROL_H

#include <QObject>
#include "../scpi/iscpi.h"

namespace QtCharts {
class QLineSeries;
class QValueAxis;
}

class ChartControl : public QObject, public ScpiUpdater
{
public:
    explicit ChartControl(QObject *chartObject, QObject *parent = nullptr);

    void parametersUpdate(const ScpiParameters &parameters);
    void dataUpdate(const ScpiData &data);

private:
    QObject *_chart {nullptr};
    QtCharts::QLineSeries *_series {nullptr};
    QtCharts::QValueAxis *_axisX {nullptr};
    QtCharts::QValueAxis *_axisY {nullptr};
};

#endif // CHARTCONTROL_H
