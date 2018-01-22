import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.EspTouch 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: espTouchPage
    enableAddAction: false
    enableMenuAction: false
    showLogo: false

    title: "Connecting controller"

	signal espTouchStarted
	signal espTouchFinshed

    EspTouch {
        id: esptouch
        password: passwordInput.text
        ssid: netMonitor.ssid
        bssid: netMonitor.bssid
        ipAddress: netMonitor.wlanIp
        onFailed: {
            espTouchPage.state = "timeout";
        }
        onHostFound: {
            espTouchPage.state = "found";
        }
    }

    onMenuSelected: {
        esptouch.running = false;
    }

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            console.log("Application state: " + state);
            if(state !== 4 /*Qt::ApplicationActive*/){
                espTouchPage.state = "prepare";
            }
        }
    }

    StackLayout {
        id: stackLayout
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Column {
            width: parent.width
            spacing: 5

            Image {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/003-wifi.svg"
            }

            Text {
                width: parent.width
                text: "<b>WiFi connection setup</b>"
                font.pixelSize: parent.width/16
                textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                height: espTouchPage.height * 0.02
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Text {
                id: ssidIntro
                width: parent.width
                text: "<b>Check that the current WiFi network is the network you want connect WiShadeController to!</b><br><br>Try to use the same network for all Controllers in singe location."
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                id: rectangle
                width: parent.width
                height: espTouchPage.height * 0.08
                color: "#00000000"
                border.color: "#00000000"

                Text {
                    id: ssid
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    font.pixelSize: parent.height / 3
                    text: netMonitor.ssid + " / " + netMonitor.bssid
                    font.bold: true
                    anchors.topMargin: 12
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: ssid.bottom
                    anchors.topMargin: 2
                    width: parent.width - 20
                    height: 2
                    color: "#000000"
                }
            }



            Rectangle {
                id: rectangle1
                height: espTouchPage.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"

                TextField {
                    id: passwordInput
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: parent.height / 3
                    background: Text {
                        anchors.fill: parent
                        visible: !(parent.focus || parent.text.length > 0)
                        color: "#b1b1b1"
                        text: "input password"
                        verticalAlignment: Text.AlignVCenter
                        font.italic: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: passwordInput.bottom
                    anchors.topMargin: 0
                    width: parent.width - 20
                    height: 2
                    color: "#000000"
                }

            }
        }

        Column {
            width: parent.width
            spacing: 5

            BusyIndicator {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                running: true
            }

            Text {
                width: parent.width
                text: "<b>Connecting controller...</b>"
                font.pixelSize: parent.width/16
                textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

        }

        Column {
            width: parent.width
            spacing: 5

            Image {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/002-001-computer-1.svg"
            }

            Text {
                width: parent.width
                text: "<b>Controller connected!</b>"
                font.pixelSize: parent.width/16
                textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                height: espTouchPage.height * 0.02
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Rectangle {
                height: espTouchPage.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"

                Text {
                    id: controllerMac
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: parent.height * 0.4
                    font.bold: true
                    text: esptouch.hostMac
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: passwordInput.bottom
                    anchors.topMargin: 0
                    width: parent.width - 20
                    height: 2
                    color: "#000000"
                }
            }

            Rectangle {
                height: espTouchPage.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"

                Text {
                    id: controllerIP
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: parent.height * 0.4
                    font.bold: true
                    text: esptouch.hostIP
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: passwordInput.bottom
                    anchors.topMargin: 0
                    width: parent.width - 20
                    height: 2
                    color: "#000000"
                }
            }
        }

        Column {
            width: parent.width
            spacing: 5

            Image {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/002-computer.svg"
            }

            Text {
                width: parent.width
                text: "<b>Connection has timed out.</b>"
                font.pixelSize: parent.width/16
                textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                text: "Check credentials, set your controller in Setup mode again and do retry."
                font.pixelSize: parent.width/16
                textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

    }

    Rectangle {
        id: actionButton
        property string title
        anchors.top: stackLayout.bottom
        anchors.left: stackLayout.left
        anchors.right: stackLayout.right
        anchors.topMargin: 16

        color: DefTheme.mainColorLight
        border.color: DefTheme.mainColorLight

        height: espTouchPage.height * 0.08
        radius: height * 0.4

        Text {
            color: DefTheme.mainTextColor
            anchors.centerIn: parent
            font.pixelSize: parent.height / 2
            text: actionButton.title
        }
        MouseArea {
            id: actionButtonMouseArea
            anchors.fill: parent
        }
    }

    Timer {
        id: espTouchTimeout
        repeat: false
        interval: 30000
        onTriggered: {
            espTouchPage.state = "timeout"
        }
    }


    states: [
        State {
            name: "prepare"
            PropertyChanges {
                target: stackLayout
                currentIndex: 0
            }
            PropertyChanges {
                target: actionButtonMouseArea
                onClicked: {
                    espTouchPage.state = "transferring";
                }
            }
            PropertyChanges {
                target: actionButton
                title: "Start"
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
            PropertyChanges {
                target: passwordInput
                focus: false
            }
        },
        State {
            name: "transferring"
            PropertyChanges {
                target: stackLayout
                currentIndex: 1
            }
            PropertyChanges {
                target: passwordInput
                focus: false
            }
            PropertyChanges {
                target: actionButtonMouseArea
                onClicked: {
                    espTouchPage.state = "prepare"
                }
            }
            PropertyChanges {
                target: actionButton
                title: "Stop"
            }
            PropertyChanges {
                target: espTouchTimeout
                running: true
            }
            PropertyChanges {
                target: esptouch
                running: true
            }
            PropertyChanges {
                target: esptouch
                onHostFound: {
                    espTouchPage.state = "found"
                }
            }
        },
        State {
            name: "found"
            PropertyChanges {
                target: stackLayout
                currentIndex: 2
            }
            PropertyChanges {
                target: actionButtonMouseArea
                onClicked: {
                    espTouchPage.state = "transferring"
                }
            }
            PropertyChanges {
                target: actionButton
                title: "Connect more"
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
        },
        State {
            name: "timeout"
            PropertyChanges {
                target: stackLayout
                currentIndex: 3
            }
            PropertyChanges {
                target: actionButtonMouseArea
                onClicked: {
                    espTouchPage.state = "prepare"
                }
            }
            PropertyChanges {
                target: actionButton
                title: "Retry"
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
        }
    ]

    state: "prepare"

}
