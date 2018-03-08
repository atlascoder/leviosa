import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import com.atlascoder.EspTouch 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem

    enableMenuAction: true

    title: "Initial Setup"
    showLogo: false

    showStatusText: true
    statusText: netMonitor.onWlan ? "via WiFi" : "via Internet"

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
            rootItem.state = "found";
        }
    }

    onMenuSelected: {
        esptouch.running = false;
    }

    onMenuClicked: {
        drawer.open()
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
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: "img/password.png"
            }

            Text {
                width: parent.width
                text: "Provide WiFi credentials"
                font.pixelSize: parent.width/16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Rectangle {
                height: rootItem.height * 0.02
                width: parent.width
                color: "#00000000"
                border.color: "#00000000"
            }

            Text {
                id: ssidIntro
                width: parent.width
                text: "Check that the current WiFi network is the really network you want connect WiShadeController to!"
                font.pixelSize: parent.width / 20
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.bold: true
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
                text: "Check credentials, set your controller in Setup mode again and do retry."
                font.pixelSize: parent.width/16
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
                text: "You need to setup you first Leviosa WiShadeController now, but it is possible only via WiFi network."
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
        anchors.top: stackLayout.bottom
        anchors.left: stackLayout.left
        anchors.right: stackLayout.right
        anchors.topMargin: 16
    }

    Timer {
        id: espTouchTimeout
        repeat: false
        interval: 30000
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

    Drawer {
        id: drawer
        width:  0.66 * applicationWindow.width
        height: applicationWindow.height
        edge: Qt.LeftEdge
        interactive: true

        background: Rectangle {
            color: DefTheme.secColorLight
        }

        Column {
            anchors.fill: parent
            anchors.margins: parent.width / 20
            spacing: 6

            Text{
                height: parent.width / 8
                anchors.horizontalCenter: parent.horizontalCenter
                verticalAlignment: Qt.AlignVCenter
                text: currentUser.email
                font.pixelSize: 16
                color: DefTheme.secTextColor
                font.bold: true
            }

            Rectangle {
                width: parent.width
                height: 2
                color: DefTheme.secColorDark
            }

            Button {
                width: parent.width
                text: "Open WebSite"
                onClicked: {
                    Qt.openUrlExternally("https://leviosashades.com") ;
                }
            }

            Button {
                width: parent.width
                text: "Call to Support"
                onClicked: {
                    Qt.openUrlExternally("tel:%1".arg("+19802061260")) ;
                }
            }

            Rectangle {
                width: parent.width
                height: 2
                color: DefTheme.secColorDark
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Sign Out"
                font.bold: true
                onClicked: {
                    currentUser.signOut();
                    drawer.close();
                    drawer.interactive = false;
                    openWelcomePage()
                }
            }
        }

        Text{
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 10
            text: "version: " + userData.version
            font.pixelSize: 10
            font.italic: true
            color: "#a0000000"
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
                onHostFound:  rootItem.state = "found"
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
                    userData.addFirstController(esptouch.hostMac, netMonitor.bssid)
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
