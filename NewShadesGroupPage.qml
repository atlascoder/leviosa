import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ShadesGroupsModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: newShadesGroupPage

    property int controllerId : -1

    title: "New Shades Group"
    enableMenuAction: false
    enableAddAction: false

    ShadesGroupsModel {
        id: shadesGroupsModel
        controllerId: newShadesGroupPage.controllerId
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
            id: groupName
			width: parent.width
			height: 40
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
			font.pixelSize: height*0.6
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
                    shadesGroupsModel.addShadesGroupWithData(newShadesGroupPage.controllerId, groupName.text);
                    menuClicked();
                }
			}

		}
	}

}
