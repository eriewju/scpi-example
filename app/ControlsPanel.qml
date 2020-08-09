import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: root
    implicitWidth: 200

    property QtObject scpi
    enabled: scpi

    QtObject {
        id: _private
        readonly property QtObject scpi: root.scpi ? root.scpi : _private.scpiStub
        readonly property QtObject scpiStub: QtObject {
            property string host
            property int port
            property bool isConnected: false
            property bool workStatus: false

            property string deviceInfo: qsTr("No device")
            property real minFrequency: 1
            property real maxFrequency: 9
            property int pointNumber: 100
            property int bandWidth: 100
        }
    }

    ColumnLayout {
        spacing: 20
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            margins: 8
        }

        GroupBox {
            // title: qsTr("Connection")
            label: Label { text: qsTr("Connection") }
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    enabled: !_private.scpi.isConnected

                    Label { text: qsTr("Host:") }
                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        text: _private.scpi.host
                        onEditingFinished: _private.scpi.host = text
                    }

                    Label { text: qsTr("Port:") }
                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        text: _private.scpi.port
                        onEditingFinished: _private.scpi.port = text
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: _private.scpi.isConnected ? qsTr("Disconnect") : qsTr("Connect")
                    onClicked: {
                        if (_private.scpi.isConnected)
                            _private.scpi.disconnectFromHost()
                        else
                            _private.scpi.connectToHost()
                    }
                }
            }
        }

        GroupBox {
            // title: qsTr("Parameters")
            label: Label { text: qsTr("Parameters") }
            enabled: _private.scpi.isConnected
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                Label {
                    text: qsTr("Start frequency (GHz)")
                }
                SpinBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    to: _private.scpi.maxFrequency - 1
                    value: _private.scpi.minFrequency
                    onValueModified: _private.scpi.minFrequency = value
                }

                Label {
                    text: qsTr("Stop frequncy (GHz)")
                    Layout.topMargin: 10
                }
                SpinBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    from: _private.scpi.minFrequency + 1
                    value: _private.scpi.maxFrequency
                    onValueModified: _private.scpi.maxFrequency = value
                }

                Label {
                    text: qsTr("Points number")
                    Layout.topMargin: 10
                }
                SpinBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    to: 1000
                    editable: true
                    value: _private.scpi.pointNumber
                    onValueModified: _private.scpi.pointNumber = value
                }

                Label {
                    text: qsTr("Band width (Hz)")
                    Layout.topMargin: 10
                }
                SpinBox {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 20
                    to: 10000
                    value: _private.scpi.bandWidth
                    onValueModified: _private.scpi.bandWidth = value
                }

                Button {
                    Layout.topMargin: 10
                    Layout.fillWidth: true
                    text: _private.scpi.workStatus ? qsTr("Stop") : qsTr("Start")
                    onClicked: _private.scpi.workStatus = !_private.scpi.workStatus
                }
            }
        }
    }
}
