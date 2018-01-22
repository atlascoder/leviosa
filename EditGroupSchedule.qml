import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Extras 1.4
import com.atlascoder.ShadesGroupsModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
	id: editGroupSchedule

    enableAddAction: false
    enableMenuAction: false

    property int controllerId : -1
    property int shadesGroupId : -1

    ShadesGroupsModel {
        id: shadesGroupsModel
        controllerId: editGroupSchedule.controllerId
        selectedGroupId: editGroupSchedule.shadesGroupId
        onSelectedGroupIdChanged: {

            if(selectedGroupOpenAt >= 13*60){
                openSlider.value = (selectedGroupOpenAt - 12*60) / 60;
                openTimePM.checked = true;
            }
            else if(selectedGroupOpenAt < 60){
                openSlider.value = (selectedGroupOpenAt - 60) / 60;
                openTimePM.checked = true;
            }
            else{
                openSlider.value = selectedGroupOpenAt / 60;
                openTimePM.checked = false;
            }

            if(selectedGroupCloseAt >= 13*60){
                closeSlider.value = (selectedGroupCloseAt - 12*60) / 60;
                closeTimePM.checked = true;
            }
            else if(selectedGroupCloseAt < 60){
                closeSlider.value = (selectedGroupCloseAt - 60) / 60;
                closeTimePM.checked = true;
            }
            else{
                closeSlider.value = selectedGroupCloseAt / 60;
                closeTimePM.checked = false;
            }
        }
    }

    title: "Schedule for " + shadesGroupsModel.selectedGroupName

	Column {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 12
		spacing: 12
		Text {
			text: "Week days:"
		}
		Row {
			width: parent.width
			WeekDayCheckBox {
                id: sunday
				text: "Sun"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & 1) != 0
			}
			WeekDayCheckBox {
                id: monday
				text: "Mon"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 1)) != 0
            }
			WeekDayCheckBox {
                id: tuesday
				text: "Tue"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 2)) != 0
            }
			WeekDayCheckBox {
                id: wednesday
				text: "Wed"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 3)) != 0
            }
			WeekDayCheckBox {
                id: thursday
				text: "Thu"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 4)) != 0
            }
			WeekDayCheckBox {
                id: friday
				text: "Fri"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 5)) != 0
            }
			WeekDayCheckBox {
                id: saturday
				text: "Sat"
				width: parent.width / 7
                checked: (shadesGroupsModel.selectedGroupDays & (1 << 6)) != 0
            }
		}

		Text {
			width: parent.width
			font.underline: true
			font.bold: true
			text: "Open at"
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			id: openTimeItem
			width: parent.width
			Text{
				id: openTimeDelim
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: parent.width / 10
				text: ":"
			}
			Text{
				anchors.right: openTimeDelim.left
				font.pixelSize: parent.width / 10
				text: Math.floor(openSlider.value)
			}
			Text{
				anchors.left: openTimeDelim.right
				font.pixelSize: parent.width / 10
				text: Math.round(60*(openSlider.value % 1))
			}
			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				Text{
					text: "AM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 12
					rightPadding: 3
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: openTimePM.checked ? 0.3 : 1
				}
				Switch {
					id: openTimePM
					width: openTimeItem.width / 6
					height: width / 3
					anchors.verticalCenter: parent.verticalCenter
					checked: false
				}
				Text{
					text: "PM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 3
					rightPadding: 12
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: openTimePM.checked ? 1 : 0.3
				}
			}
			height: childrenRect.height
		}

		Slider {
			id: openSlider
			from: 1
			to: 12.75
			stepSize: 0.25
			value: 7
			width: parent.width
		}


		Text {
			width: parent.width
			font.underline: true
			font.bold: true
			text: "Close at"
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			id: closeTimeItem
			width: parent.width
			Text{
				id: closeTimeDelim
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: parent.width / 10
				text: ":"
			}
			Text{
				anchors.right: closeTimeDelim.left
				font.pixelSize: parent.width / 10
				text: Math.floor(closeSlider.value)
			}
			Text{
				anchors.left: closeTimeDelim.right
				font.pixelSize: parent.width / 10
				text: Math.round(60*(closeSlider.value % 1))
			}
			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				Text{
					text: "AM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 12
					rightPadding: 3
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: closeTimePM.checked ? 0.3 : 1
				}
				Switch {
					id: closeTimePM
					width: closeTimeItem.width / 6
					height: width / 3
					anchors.verticalCenter: parent.verticalCenter
					checked: true
				}
				Text{
					text: "PM"
					height: parent.height
					font.pixelSize: parent.width / 10
					leftPadding: 3
					rightPadding: 12
					verticalAlignment: Qt.AlignVCenter
					font.bold: true
					opacity: closeTimePM.checked ? 1 : 0.3
				}
			}
			height: childrenRect.height
		}

		Slider {
			id: closeSlider
			from: 1
			to: 12.75
			stepSize: 0.25
			value: 7
			width: parent.width
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
                    var days = 0;
                    if(sunday.checked) days = 1;
                    if(monday.checked) days |= 1<<1;
                    if(tuesday.checked) days |= 1<<2;
                    if(wednesday.checked) days |= 1<<3;
                    if(thursday.checked) days |= 1<<4;
                    if(friday.checked) days |= 1<<5;
                    if(saturday.checked) days |= 1<<6;
                    shadesGroupsModel.selectedGroupDays = days;
                    if(openTimePM.checked){
                        shadesGroupsModel.selectedGroupOpenAt = ((openSlider.value + 12) * 60)%(24*60)
                    }
                    else{
                        shadesGroupsModel.selectedGroupOpenAt = (openSlider - 1) * 60;
                    }
                    if(closeTimePM.checked){
                        shadesGroupsModel.selectedGroupCloseAt = ((closeSlider.value + 12) * 60)%(24*60)
                    }
                    else{
                        shadesGroupsModel.selectedGroupCloseAt = (closeSlider - 1) * 60;
                    }
                    menuClicked();
                }
			}

		}
	}

}
