import QtQuick 2.7
import QtQuick.Controls 2.2

ListView {
	anchors.fill: parent
	spacing: 16
	headerPositioning: ListView.InlineHeader

	signal editShadesGroup(string n)
	signal editGroupSchedule(string n)

	model: shadeGroups

	header: Item {
		id: headerItem
		width: parent.width
		height: 16
	}

	footer: AllShadesControl {
		z: 2
	}

	footerPositioning: ListView.OverlayFooter

	property ListModel shadeGroups: ListModel {}


	delegate: Item {
		width: parent.width
		ShadeControl {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width*0.95
			title: name
			onEdit: editShadesGroup(name)
			onOpenSchedule: editGroupSchedule(name)
		}
		height: childrenRect.height
	}
}
