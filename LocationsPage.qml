import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.LocationsModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem
	title: "Locations"
    enableAddAction: false

    property bool swiped : false

    signal openLocation(string uuid, string tzone, string bssid)
    signal editLocation(string uuid)

    showStatusText: true
    statusText: netMonitor.onWlan ? "via WiFi" : "via Internet"

    showSubTitle: true
    subTitle: "Loading..."

    Timezones {
        id: tzones
    }


    LocationsModel {
        id: locationModel
        currentBssid: netMonitor.bssid
    }

    function init(){
        locationModel.updateModel();
        if(netMonitor.connected){
            userData.sync();
        }
        swiper.start();
    }

    Timer {
        id: swiper
        interval: 500
        repeat: false
        onTriggered:{
            if((locationModel.rowCount() === 1) && !swiped){
                swiped = true;
                openLocation(locationModel.uuidByIndex(0), locationModel.timezone, locationModel.bssidByIndex(0));
            }
        }
    }

    function pause(){

    }

	GridView {
		id: gridView
		anchors.fill: parent
		cellWidth: {
			if (parent.width <= 240)
				return parent.width
			else{
				var n = 1;
				while(parent.width / n > 240){ n++; }
				return Math.round(parent.width/n);
			}
		}
		cellHeight: cellWidth
        model: locationModel
		delegate: Item {
			id: locItem
			width: gridView.cellWidth
			height: gridView.cellHeight

            property bool isOnWlan : netMonitor.onWlan && netMonitor.bssid === model.bssid

			Button {
				id: itemRect
				anchors.fill: parent
				anchors.margins: 10
				background: Rectangle {
					radius: 4
					layer.enabled: true
					layer.effect: DropShadow {
						radius: itemRect.down ? 1 : 3
						transparentBorder: true
					}
					color: DefTheme.mainColorLight
				}


                Timer {
                    interval: 500
                    repeat: true
                    property string timezoneName : tzones.getSignature(utcOffset)
                    property int tOff: utcOffset*1000
                    onTriggered: {
                        var d = new Date();
                        timeline.text = timezoneName + " " + (new Date(d.getTime() + tOff + d.getTimezoneOffset()*60000).toLocaleTimeString("en-US"))
                    }
                    running: true
                }

				Text {
					id: timeline
					width: parent.width
					anchors.top: parent.top
					anchors.margins: height / 3
					horizontalAlignment: Qt.AlignHCenter
					color: DefTheme.mainTextColor
					font.pixelSize: width / 10
				}

				Text {
					id: nameLine
					width: parent.width
					anchors.bottom: parent.bottom
					anchors.margins: 4
					horizontalAlignment: Qt.AlignHCenter
                    text: name
					font.pixelSize: parent.height / 8
					color: DefTheme.mainTextColor
					elide: Text.ElideRight
				}
				Item {
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.top: timeline.bottom
					anchors.bottom: nameLine.top
                    anchors.margins: parent.width / 8
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
                    openLocation(uuid, locationModel.timezone, bssid);
				}
				onPressAndHold: {
                    if(rootItem.state === "Synced")
                    editLocation(uuid);
				}
			}
		}
	}

    PropertyAnimation {
        id: syncing
    }

    onMenuClicked: drawer.open()

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
                text: "Add this location"
                onClicked: {
                    drawer.close()
                    openNewLocationPage()
                }
                visible: netMonitor.onWlan
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

            Button {
                width: parent.width
                text: "Change password"
                onClicked: {
                    drawer.close()
                    openChangePasswordPage()
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
    }

    states: [
        State {
            name: "Syncing"
            when: userData.isSyncing
            PropertyChanges {
                target: rootItem
                enableAddAction: false
            }
            StateChangeScript {
                script: {
                    console.log("Status: Syncing")
                }
            }
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
                enableAddAction: true
            }
            StateChangeScript {
                script: {
                    console.log("Status: Synced")
                }
            }
            PropertyChanges {
                target: rootItem
                subTitle: "Data synced"
            }
        }
    ]

}
