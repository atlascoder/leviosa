import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import com.atlascoder.LocationsModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: editLocation
    enableAddAction: false
    enableMenuAction: false
    title: "Edit location"

    property alias locationId : locationModel.selectedLocationId

    LocationsModel {
        id: locationModel
    }

	Column {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
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
            text: "WiFi network name:"
        }

        Text {
            id: ssid
            width: parent.width
            height: 40
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: height*0.6
            color: DefTheme.mainDisabledTextColor
//            text: locationModel.selectedSsid
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

		Item {
			width: parent.width
			height: childrenRect.height

			ActionButton {
				id: cancelBtn
				text: "Cancel"
				anchors.top: parent.top
				anchors.right: parent.horizontalCenter
				anchors.left: parent.left
				anchors.leftMargin: 6
				height: 40
                onClicked: menuClicked()
			}

			ActionButton {
				id: saveBtn
				text: "Save"
				anchors.top: parent.top
				anchors.left: parent.horizontalCenter
				anchors.right: parent.right
				anchors.leftMargin: 6
				height: 40
                onClicked: {
                    locationModel.updateLocationWithData(
                                locationModel.selectedLocationId,
                                locationName.text,
                                tzones.model.get(timezoneCB.currentIndex).utcOffset,
                                bssid.text,
                                ssid.text,
                                showBefore.currentIndex
                    );
                    menuClicked();
                }
			}
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
                    locationModel.markDeleted(locationModel.selectedLocationId);
                    menuClicked();
                }
            }
        }

	}

}
