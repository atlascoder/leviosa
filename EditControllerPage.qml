import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ControllersModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: editController

    property alias locationUuid : controllersModel.locationUuid
    property alias controllerMac : controllersModel.selectedControllerMac

    enableMenuAction: false

    title: "Edit controller"

    ControllersModel {
        id: controllersModel
        onSelectedControllerMacChanged: {
            showBefore.model = positionOrder;
            showBefore.visible = rowCount();
            showBefore.currentIndex = selectedControllerPosition;
        }
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
            id: controllerName
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
			background: Rectangle {
				anchors.fill: parent
				color: DefTheme.mainInputBg
			}
            text: controllersModel.selectedControllerName
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
            text: controllersModel.selectedControllerMac
		}

        Text {
            text: "Show before:"
            visible: controllersModel.rowCount() > 1
        }

        ComboBox {
            id: showBefore
            width: parent.width
            height: 40
            model: controllersModel.positionOrder
            visible: controllersModel.rowCount() > 1
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
                    controllersModel.updateControllerWithData(editController.controllerMac, controllerName.text, showBefore.currentIndex);
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
                    controllersModel.remove(editController.controllerMac);
                    menuClicked();
                }
            }
        }

	}

}
