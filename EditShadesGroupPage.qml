import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import com.atlascoder.ShadesGroupsModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: rootItem

    title: "Edit shade group"

    enableMenuAction: false
    enableAddAction: false
    showLogo: false

    property int controllerId : -1
    property int shadesGroupId : -1

    property int openAt
    property int closeAt
    property alias openTimeDelimFontpixelSize: openTimeDelim.font.pixelSize

    ShadesGroupsModel {
        id: shadesGroupsModel
        controllerId: rootItem.controllerId
        selectedGroupId: rootItem.shadesGroupId
        onSelectedGroupIdChanged: {
            positionCB.model = positionOrder;
            positionCB.currentIndex = selectedGroupPosition;
            positionCB.visible = rowCount() > 1;
            positionCaption.visible = rowCount() > 1;
        }
    }
	
    TabBar {
        id: editorTabs
        anchors.top: parent.top
        width: parent.width
        currentIndex: pager.currentIndex
        z: 1
        TabButton {
            text: "Schedule"
            width: editorTabs.width / 2
        }
        TabButton {
            text: "Control"
            width: editorTabs.width / 2
        }
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
                rootItem.visible = false;
                var days = 0;
                if(sunday.checked) days = 1;
                if(monday.checked) days |= 1<<1;
                if(tuesday.checked) days |= 1<<2;
                if(wednesday.checked) days |= 1<<3;
                if(thursday.checked) days |= 1<<4;
                if(friday.checked) days |= 1<<5;
                if(saturday.checked) days |= 1<<6;
                var open_at_us = openTimePM.checked ? openSlider.value : -openSlider.value;
                var close_at_us = closeTimePM.checked ? closeSlider.value : -closeSlider.value;
                if(setForAll.checked){
                    shadesGroupsModel.setScheduleForCurrentController(days, open_at_us, close_at_us);
                }
                else{
                    shadesGroupsModel.selectedGroupDays = days;
                    shadesGroupsModel.selectedOpenAtUS = open_at_us;
                    shadesGroupsModel.selectedCloseAtUS = close_at_us;
                }
                shadesGroupsModel.selectedGroupName = groupName.text;
                shadesGroupsModel.selectedGroupPosition = positionCB.currentIndex;
                menuClicked();
            }
        }

    }


    SwipeView {
        id: pager
        anchors.top: editorTabs.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: editorTabs.currentIndex

        Flickable {

            contentWidth: schedulePane.width
            contentHeight: schedulePane.implicitHeight

            ScrollBar.vertical: ScrollBar {}

            Pane {
                id: schedulePane
                width: pager.width
                background: Rectangle {
                    anchors.fill: parent
                    color: "#00000000"
                }

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 6
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

                        Text {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: parent.width / 24
                            text: "<b>" + locationModel.selectedTimezoneName + "</b> time"
                        }
                        Text{
                            id: openTimeDelim
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pixelSize: parent.width / 11
                            text: ":"
                        }
                        Text{
                            anchors.right: openTimeDelim.left
                            font.pixelSize: parent.width / 11
                            text: Math.floor(openSlider.value)
                        }
                        Text{
                            anchors.left: openTimeDelim.right
                            font.pixelSize: parent.width / 11
                            text: openSlider.value % 1 == 0 ? "00" : Math.floor((openSlider.value % 1.0) * 60)
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
                                width: openTimeItem.width / 8
                                height: width / 4
                                anchors.verticalCenter: parent.verticalCenter
                                checked: shadesGroupsModel.selectedOpenAtUS  > 0
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
                        value: shadesGroupsModel.selectedOpenAtUS > 0 ? shadesGroupsModel.selectedOpenAtUS : -shadesGroupsModel.selectedOpenAtUS
                        width: parent.width
                        height: 30
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
                            font.pixelSize: parent.width / 11
                            text: ":"
                        }
                        Text{
                            anchors.right: closeTimeDelim.left
                            font.pixelSize: parent.width / 11
                            text: Math.floor(Math.abs(closeSlider.value))
                        }
                        Text{
                            anchors.left: closeTimeDelim.right
                            font.pixelSize: parent.width / 11
                            text: closeSlider.value % 1 == 0 ? "00" : Math.floor((closeSlider.value % 1.0) * 60)
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
                                width: closeTimeItem.width / 8
                                height: width / 4
                                anchors.verticalCenter: parent.verticalCenter
                                checked: shadesGroupsModel.selectedCloseAtUS  > 0
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
                        value: shadesGroupsModel.selectedCloseAtUS > 0 ? shadesGroupsModel.selectedCloseAtUS : -shadesGroupsModel.selectedCloseAtUS
                        width: parent.width
                        height: 30
                    }

                    Row{
                        CheckBox {
                            id: setForAll
                            font.bold: true
                        }
                        Text {
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            elide: Text.ElideRight
                            text: "set for all shades"
                            font.pixelSize: DefTheme.toolbarHeight * 0.35
                            font.bold: true
                        }
                    }


                }
            }
        }

        Item {
            Column {
                anchors.fill: parent
                anchors.margins: 16
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
                    text: shadesGroupsModel.selectedGroupName
                }

                Text {
                    id: positionCaption
                    text: "Position:"
                    visible: false
                }

                ComboBox {
                    id: positionCB
                    width: parent.width
                    height: 40
                    visible: false
                    displayText: currentText
                    contentItem: Text {
                        leftPadding: positionCB.indicator.width + positionCB.spacing
                        rightPadding: positionCB.spacing
                        font.pixelSize: height * 0.6
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        text: positionCB.displayText
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
                            shadesGroupsModel.removeShadesGroup(rootItem.shadesGroupId);
                            menuClicked();
                        }
                    }
                }
            }
        }

    }

}
