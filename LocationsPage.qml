import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.LocationsModel 1.0
import com.atlascoder.TimeZoneModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem
    title: "Networks"

    signal openLocation(string uuid)
    signal editLocation(string uuid)

    showStatusText: true
    statusText: netMonitor.onWlan ? "via WiFi" : "via Internet"

    showSubTitle: true
    subTitle: "Loading..."

    TimeZoneModel {
        id: tzones
    }

    LocationsModel {
        id: locationsModel
    }

    Connections {
        target: alertBox
        onActiveChanged: {
            if (alertBox.active) {
                allShadesControl.reset()
            }
        }
    }

    StackView.onActivated: {
        locationsModel.reloadData()
        locationsModel.active = true
        allShadesControl.reset()
    }

    StackView.onDeactivating: {
        locationsModel.active = false
    }

    Connections {
        target: qGuiApp
        onApplicationStateChanged: {
            if(state === Qt.ApplicationActive){
                locationsModel.active = rootItem.StackView.status === StackView.Active
            }
            else {
                locationsModel.active = false
            }
        }
    }

    AllShadesControl {
        id: allShadesControl
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        z:1
        onCmdShade: function(shade_state){
            locationsModel.commandShades(shade_state);
        }
    }

	GridView {
		id: gridView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: allShadesControl.top
        anchors.margins: 6

        cellWidth: height > width ? (width / 2) : (width / 3)
        cellHeight: height > width ? (height / 3) : (height / 2)

        model: locationsModel
        delegate: ItemDelegate {
            id: locItem
            width: gridView.cellWidth
            height: gridView.cellHeight
            contentItem: Item {}

            property bool isOnWlan : netMonitor.onWlan && netMonitor.bssid === model.bssid

            Button {
                id: itemRect
                anchors.fill: parent
                anchors.margins: 6
                background: Rectangle {
                    radius: 6
                    layer.enabled: true
                    layer.effect: DropShadow {
                        radius: itemRect.down ? 1 : 3
                        transparentBorder: true
                    }
                    color: DefTheme.mainColorLight
                }

//                Text {
//                    id: timeline
//                    width: parent.width
//                    anchors.top: parent.top
//                    anchors.margins: height / 3
//                    horizontalAlignment: Qt.AlignHCenter
//                    color: DefTheme.mainTextColor
//                    font.pixelSize: parent.height / 10
//                    text: locationTime
//                    elide: Text.ElideRight
//                }

                Text {
                    id: nameLine
                    width: parent.width
                    anchors.bottom: parent.bottom
                    anchors.margins: 4
                    horizontalAlignment: Qt.AlignHCenter
                    text: name
                    font.pixelSize: parent.height / 7
                    color: DefTheme.mainTextColor
                    elide: Text.ElideRight
                }
                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
//                    anchors.top: timeline.bottom
                    anchors.top: parent.top
                    anchors.bottom: nameLine.top
                    anchors.margins: parent.width / 12
                    anchors.topMargin: parent.height / 6
                    Image {
                        id: icon
                        anchors.fill: parent
                        verticalAlignment: Qt.AlignVCenter
                        horizontalAlignment: Qt.AlignHCenter
                        fillMode: Image.PreserveAspectFit
                        source: isOnWlan ? "img/001-computer-1.svg" : "img/001-internet.svg"
                    }
                    ColorOverlay {
                        anchors.fill: parent
                        source: icon
                        color: DefTheme.mainPositiveAccent
                        visible: isOnWlan
                    }
                }
                onClicked: {
                    openLocation(uuid);
                }
                onPressAndHold: {
                    if(rootItem.state === "Synced")
                    editLocation(uuid);
                }
            }
        }
	}

    states: [
        State {
            name: "Syncing"
            when: userData.isSyncing
            PropertyChanges {
                target: rootItem
                subTitle: "Syncing data..."
            }
        },
        State {
            name: "Synced"
            when: !userData.isSyncing
            PropertyChanges {
                target: rootItem
                subTitle: "Data synced"
            }
        }
    ]

}
