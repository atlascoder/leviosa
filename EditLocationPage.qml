import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import com.atlascoder.LocationsModel 1.0
import com.atlascoder.ControllersModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: editLocation
    enableAddAction: false
    enableMenuAction: false
    title: "Edit location"

    property alias locationUuid : locationModel.selectedLocationUuid

    signal setupController(string uuid)

    LocationsModel {
        id: locationModel
    }

    TabBar {
        id: editorTabs
        anchors.top: parent.top
        width: parent.width
        currentIndex: pager.currentIndex
        z: 1
        TabButton {
            text: "Name"
            width: editorTabs.width / 2
        }
        TabButton {
            text: "Controllers"
            width: editorTabs.width / 2
        }
    }

    SwipeView {
        id: pager
        anchors.top: editorTabs.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        currentIndex: editorTabs.currentIndex

        Item {
            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12
                Text {
                    text: "Name:"
                }
                TextField {
                    id: locationName
                    width: parent.width
                    height: 40
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font.pixelSize: height*0.6
                    background: Rectangle {
                        anchors.fill: parent
                        color: DefTheme.mainInputBg
                    }
                    text: locationModel.selectedLocationName
                }

                Text {
                    text: "Time zone:"
                }

                Timezones {
                    id: tzones
                }

                ComboBox {
                    id: timezoneCB
                    width: parent.width
                    height: 40
                    model: tzones.model
                    textRole: "name"
                    contentItem: Text {
                        leftPadding: timezoneCB.indicator.width + timezoneCB.spacing
                        rightPadding: timezoneCB.spacing
                        text: timezoneCB.displayText
                        font.pixelSize: height * 0.6
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    background: Rectangle {
                        anchors.fill: parent
                        color: DefTheme.mainInputBg
                    }
                    currentIndex: tzones.getIndex(locationModel.selectedLocationUtcOffset)
                }


                Text {
                    text: "WiFi access point MAC:"
                }

                Text {
                    id: bssid
                    width: parent.width
                    height: 40
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font.pixelSize: height*0.6
                    color: DefTheme.mainDisabledTextColor
                    text: locationModel.selectedLocationBssid
                }

                Text {
                    text: "Show:"
                    visible: locationModel.rowCount() > 1
                }

                ComboBox {
                    id: showBefore
                    width: parent.width
                    height: 40
                    model: locationModel.positionOrder
                    visible: locationModel.rowCount() > 1
                    currentIndex: locationModel.selectedLocationPosition
                    displayText: currentText
                    contentItem: Text {
                        leftPadding: showBefore.indicator.width + showBefore.spacing
                        rightPadding: showBefore.spacing
                        font.pixelSize: height * 0.6
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        text: showBefore.displayText
                    }
                    background: Rectangle {
                        anchors.fill: parent
                        color: DefTheme.mainInputBg
                    }
                }

                Item {
                    width: parent.width
                    height: 20
                }
                Rectangle {
                    width: parent.width / 2
                    height: DefTheme.toolbarHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: DefTheme.mainNegativeAccent
                    Text {
                        anchors.centerIn: parent
                        text: "Delete?"
                        font.pixelSize: parent.height *0.4
                        color: "#ffffff"
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 4
                        font.pixelSize: parent.height * 0.2
                        font.italic: true
                        text: "Press and hold to delete"
                        color: "#ffffff"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onPressAndHold: {
                            locationModel.remove(locationModel.selectedLocationUuid);
                            menuClicked();
                        }
                    }
                }

            }
        }

        Item {
            Item {
                id: footerButton
                height: DefTheme.buttonHeight + 12
                width: parent.width
                anchors.bottom: parent.bottom

                ActionButton {
                    id: addControllerButton
                    outline: true
                    text: "Add controller"
                    anchors.centerIn: footerButton
                    onClicked: setupController(locationUuid)
                    visible: netMonitor.onWlan && netMonitor.bssid == locationModel.selectedLocationBssid
                }

                Text{
                    anchors.fill: footerButton
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    padding: 16
                    wrapMode: Text.WordWrap
                    text: "You can add more controllers being connected to WiFi at this location."
                    font.pixelSize: DefTheme.buttonHeight / 3
                    font.italic: true
                    font.bold: true
                    visible: !addControllerButton.visible
                }
            }
            Flickable {
                id: controllersList
                anchors.top: parent.top
                anchors.bottom: footerButton.top

                contentWidth: controllersColumn.width
                contentHeight: controllersColumn.implicitHeight

                ScrollBar.vertical: ScrollBar {}

                Column{
                    id: controllersColumn
                    width: pager.width
                    Repeater {
                        model: controllersModel
                        Item {
                            height: DefTheme.toolbarHeight * 2
                            width: parent.width - 24
                            anchors.horizontalCenter: parent.horizontalCenter
                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: mac
                                font.bold: true
                                font.pixelSize: DefTheme.toolbarHeight / 3
                            }
                            DeleteButton {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                onPressAndHold: {
                                    controllersModel.remove(mac)
                                    menuClicked()
                                }
                            }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                height: 1
                                width: parent.width
                                color: "#a0a0a0"
                            }
                        }
                    }
                }
            }
        }
    }

    ControllersModel {
        id: controllersModel
        locationUuid: locationModel.selectedLocationUuid
    }

    footer: Rectangle {
        id: buttonsBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        width: parent.width
        height: DefTheme.toolbarHeight * 1.5
        color: DefTheme.mainColorBg

        ActionButton {
            text: "Cancel"
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width / 2 - 32
            x: 16
            height: 40
            onClicked: menuClicked()
        }

        ActionButton {
            text: "Save"
            anchors.verticalCenter: parent.verticalCenter
            x: parent.width / 2 + 16
            width: parent.width / 2 - 32
            height: 40
            onClicked: {
                locationModel.updateLocationWithData(
                            locationModel.selectedLocationUuid,
                            locationName.text,
                            tzones.model.get(timezoneCB.currentIndex).utcOffset,
                            bssid.text,
                            showBefore.currentIndex
                );
                menuClicked();
            }
        }

    }

}
