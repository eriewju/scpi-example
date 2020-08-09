import QtQuick 2.12
import QtCharts 2.3

ChartView {
    legend.visible: false
    antialiasing: true
    objectName: "_chart"
    margins {
        top: 15
        left: 0
    }

    LineSeries { name: "lineSeries" }
}
