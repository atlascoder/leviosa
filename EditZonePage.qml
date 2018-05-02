import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ZoneModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: editZone

    property alias uuid : zoneModel.uuid

    signal zoneDeleted()

    enableMenuAction: false

    title: zoneModel.isNew ? "New Shade Zone" : ("Edit " + zoneModel.name)

    ZoneModel {
        id: zoneModel
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
            id: zoneName
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
			background: Rectangle {
				anchors.fill: parent
				color: DefTheme.mainInputBg
			}
            text: zoneModel.name
		}

		Text {
            text: "Controller MAC:"
		}

		Text {
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
			color: DefTheme.mainDisabledTextColor
            text: zoneModel.controllerMac
		}

        Text {
            text: "Show before:"
            visible: showBefore.visible
        }

        ComboBox {
            id: showBefore
            width: parent.width
            height: 40
            model: zoneModel.positionOrder
            visible: model.length > 1
            currentIndex: zoneModel.position
            displayText: currentText
            contentItem: Text {
                leftPadding: showBefore.indicator.width + showBefore.spacing
                rightPadding: showBefore.spacing
                font.pixelSize: height * 0.6
                text: showBefore.displayText
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
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

        DeleteButton {
            anchors.horizontalCenter: parent.horizontalCenter
            onPressAndHold: {
                zoneModel.deleteZone()
                zoneDeleted()
            }
        }
	}

    footer: Item {
        width: parent.width
        height: 64

        ActionButton {
            id: cancelBtn
            text: "Cancel"
            anchors.top: parent.top
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            anchors.margins: 12
            height: 40
            onClicked: goBack()
            outline: true
        }

        ActionButton {
            id: saveBtn
            text: "Save"
            anchors.top: parent.top
            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.margins: 12
            height: 40
            onClicked: {
                zoneModel.name = zoneName.text
                zoneModel.position = showBefore.currentIndex
                zoneModel.saveChanges()
                goBack()
            }
        }
    }
}
