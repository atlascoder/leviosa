import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.EspTouch 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem

    title: "Controller Setup"
    showLogo: false

    showStatusText: false

    signal controllerAdded()

    EspTouch {
        id: esptouch
        password: passwordInput.text
        ssid: netMonitor.ssid
        bssid: netMonitor.bssid
        ipAddress: netMonitor.wlanIp
        onFailed: {
            rootItem.state = "timeout";
        }
        onHostFound: {
            rootItem.state = "registering";
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
                rootItem.state = "prepare";
            }
        }
    }

    StackLayout {
        id: stackLayout
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40

        Column {
            width: parent.width
            spacing: 5

            Image {
                id: image
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/password.png"
            }

//            Text {
//                width: parent.width
//                text: "Provide WiFi credentials"
//                font.pixelSize: parent.width/16
//                font.bold: true
//                horizontalAlignment: Text.AlignHCenter
//                wrapMode: Text.WordWrap
//            }

            Rectangle {
                height: rootItem.height * 0.02
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Text {
                id: ssidIntro
                width: parent.width
                text: "Setup of Zone:"
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                font.bold: true
            }

            Text {
                id: ssidIntro1
                width: parent.width
                anchors.topMargin: 6
                text: "1. Ensure the Leviosa Zone is powered (green light glows on back of Zone)"
                font.pixelSize: parent.width / 20
                font.bold: false
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
            }

            Text {
                id: ssidIntro2
                width: parent.width
                anchors.topMargin: 6
                text: "2. To the left, the Yellow light should blink slowly (1/sec).\nIf not, press/hold the Reset button for 8 seconds, and wait for the yellow light to blink"
                font.pixelSize: parent.width / 20
                font.bold: false
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
            }

            Text {
                id: ssidIntro3
                width: parent.width
                anchors.topMargin: 6
                text: "3. Confirm via device Settings this App is on the same network named below, and enter network password"
                font.pixelSize: parent.width / 20
                font.bold: false
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
            }

            Rectangle {
                id: rectangle
                width: parent.width
                height: rootItem.height * 0.08
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
                    elide: Text.ElideRight
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
                height: rootItem.height * 0.08
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

            Image {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/wifi.png"
            }

            BusyIndicator {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                running: true
            }

            Text {
                id: text1
                width: parent.width
                text: "Connecting controller..."
                font.pixelSize: parent.width / 16
                font.bold: true
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
                source: "img/smile.png"
            }

            Rectangle {
                height: rootItem.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Text {
                width: parent.width
                text: "Controller connected!"
                font.pixelSize: parent.width / 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                height: rootItem.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"

            }
        }

        Column {
            width: parent.width
            spacing: 5

            Image {
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/sad.png"
            }

            Rectangle {
                height: rootItem.height * 0.08
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Text {
                width: parent.width
                text: "Connection has timed out."
                font.pixelSize: parent.width / 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                text: "Solutions:"
                font.pixelSize: parent.width / 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                id: text2
                width: parent.width
                text: "1. Ensure the controller is plugged in and controller 'power' light is on.\nThe controller must be located in an active WiFi zone"
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                width: parent.width
                text: "2. The device and Controller must connect to the same WiFi network.\nCheck your network name and password, and try again"
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Item {
                width: parent.width
                height: 16
            }

            Text {
                width: parent.width
                text: "If that does not work, press and hold reset button on Controller for 10 seconds, and retry again"
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

        Column {
            id: wlanRequiredAlert
            width: parent.width * 0.8
            height: childrenRect.height
            anchors.centerIn: parent
            spacing: width / 20

            Image {
                id: notNetworkIcon
                source: "img/information.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "WiFi connection required!"
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: parent.width / 16
                font.bold: true
            }

            Text {
                text: "You can setup Leviosa WiShadeController only via WiFi network."
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: width / 20
            }

            Text {
                text: "Please, connect this device to your WiFi network to start Setup."
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: width / 20
            }
        }
    }

    ActionButton {
        id: actionButton
        anchors.bottomMargin: height / 2
        anchors.bottom: parent.bottom
        anchors.left: stackLayout.left
        anchors.right: stackLayout.right
    }

    Timer {
        id: espTouchTimeout
        repeat: false
        interval: 60000
        onTriggered: {
            rootItem.state = "timeout"
        }
    }

    Connections {
        target: netMonitor
        onOnWlanChanged: {
            if(netMonitor.onWlan)
                rootItem.state = "prepare"
            else
                rootItem.state = "offline"
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
                target: actionButton
                text: "Start"
                visible: true
                onClicked: rootItem.state = "transferring"
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
            PropertyChanges {
                target: passwordInput
                focus: false
            }

            PropertyChanges {
                target: ssidIntro
                font.bold: false
            }

            PropertyChanges {
                target: image
                width: parent.width * 0.5
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
                target: actionButton
                text: "Stop"
                visible: true
                onClicked: rootItem.state = "prepare"
            }
            PropertyChanges {
                target: espTouchTimeout
                running: true
            }
            PropertyChanges {
                target: esptouch
                running: true
                onHostFound: rootItem.state = "registering"
            }

            PropertyChanges {
                target: text1
                text: "Searching for Controller...\nPlease, wait up to 1 minute.."
            }
        },
        State{
            name: "registering"
            PropertyChanges {
                target: stackLayout
                currentIndex: 1
            }
            PropertyChanges {
                target: passwordInput
                focus: false
            }
            PropertyChanges {
                target: actionButton
                text: "Stop"
                visible: false
                onClicked: rootItem.state = "prepare"
            }
            PropertyChanges {
                target: espTouchTimeout
                running: false
            }
            StateChangeScript {
                script: {
                    userData.setupControllerKeys(esptouch.hostMac, esptouch.hostIP, netMonitor.bssid)
                    userData.onControllerSetupSuccessful.connect(function(){
                        rootItem.state = "found"
                    })
                    userData.onControllerSetupFailed.connect(function(errmsg){
                        rootItem.state = "timeout"
                    })
                }
            }

            PropertyChanges {
                target: text1
                text: "Controller Discovered.\nConfiguring Controller...\n\nA few more moments.."
            }
        },
        State {
            name: "found"
            PropertyChanges {
                target: stackLayout
                currentIndex: 2
            }
            PropertyChanges {
                target: actionButton
                text: "Go to location"
                visible: false
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
            StateChangeScript {
                script: {
                    userData.setupController(esptouch.hostMac, netMonitor.bssid)
                    controllerAdded()
                }
            }
        },
        State {
            name: "timeout"
            PropertyChanges {
                target: stackLayout
                currentIndex: 3
            }
            PropertyChanges {
                target: actionButton
                text: "Retry"
                visible: true
                onClicked: rootItem.state = "prepare"
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
        },
        State {
            name: "offline"
            PropertyChanges {
                target: stackLayout
                currentIndex: 4
            }
            PropertyChanges {
                target: esptouch
                running: false
            }
            PropertyChanges {
                target: actionButton
                visible: false
            }
        }
    ]

    state: "prepare"

}
