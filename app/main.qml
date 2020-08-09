import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.3

Window {
    visible: true
    minimumWidth: 800
    minimumHeight: 520
    title: qsTr("Test")

    RowLayout {
        anchors.fill: parent
        ControlsPanel {
            scpi: scpiClient
            Layout.preferredWidth: 250
            Layout.fillHeight: true
        }
        ChartPanel {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
