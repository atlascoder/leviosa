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

    property string controllerMac
    property int selectedChannel
    property alias controllerIp : shadesGroupsModel.ipAddress

    property int openAt
    property int closeAt
    property alias openTimeDelimFontpixelSize: openTimeDelim.font.pixelSize

    property bool processing : false

    function init() {
        shadesGroupsModel.controllerMac = rootItem.controllerMac;
        shadesGroupsModel.selectedChannel = rootItem.selectedChannel;
        showBefore.visible = shadesGroupsModel.count > 1;
        showBefore.model = shadesGroupsModel.positionOrder;
        showBefore.currentIndex = shadesGroupsModel.selectedGroupPosition;
    }

    ShadesGroupsModel {
        id: shadesGroupsModel
        onScheduleSet: menuClicked()
        onScheduleSetFailed: {
            processing = false
            alertScheduleNotSet.visible = true
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

    AlertMessage {
        id: alertScheduleNotSet
        message: "Schedule was not set. Probably due to controller connection failure."
        visible: false
        z: 2
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
            enabled: !busyIndi.visible
        }

        ActionButton {
            text: "Save"
            anchors.verticalCenter: parent.verticalCenter
            x: parent.width / 2 + 16
            width: parent.width / 2 - 32
            height: 40
            enabled: !busyIndi.visible
            onClicked: {
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
                processing = true;
                shadesGroupsModel.updateShadeGroupsWithData(selectedChannel, groupName.text, showBefore.currentIndex, open_at_us, close_at_us, days, setForAll.checked);
            }
        }

    }

    Rectangle {
        id: busyIndi
        anchors.fill: parent
        color: "#ebffffff"
        visible: processing
        z: 1
        BusyIndicator {
            anchors.centerIn: parent
            running: parent.visible
        }
        MouseArea {
            anchors.fill: parent
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
                        font.pointSize: width / 22
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Item {
                        id: openTimeItem
                        width: parent.width

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
                                font.pixelSize: parent.width / 8
                                leftPadding: 12
                                rightPadding: 3
                                verticalAlignment: Qt.AlignVCenter
                                font.bold: true
                                opacity: openTimePM.checked ? 0.3 : 1
                            }
                            Switch {
                                id: openTimePM
                                width: openTimeItem.width / 8
                                height: width / 2
                                anchors.verticalCenter: parent.verticalCenter
                                checked: shadesGroupsModel.selectedOpenAtUS  > 0

                                background: Rectangle {
                                    implicitWidth: openTimePM.width
                                    implicitHeight: openTimePM.height/2
                                    y: height / 2
                                    radius: height / 2
                                    color: openTimePM.checked ? "#000000" : "#ffffff"
                                    border.color: openTimePM.checked ? "#808080" : "#cccccc"
                                }

                                indicator: Rectangle {
                                    x: openTimePM.checked ? openTimePM.width - width : 0
                                    height: openTimePM.height
                                    width: openTimePM.height
                                    radius: height / 2
                                    color: openTimePM.down ? "#cccccc" : "#ffffff"
                                    border.color: openTimePM.checked ? "#000000" : "#999999"
                                }
                            }
                            Text{
                                text: "PM"
                                height: parent.height
                                font.pixelSize: parent.width / 8
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
                        font.pixelSize: width / 22
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
                                font.pixelSize: parent.width / 8
                                leftPadding: 12
                                rightPadding: 3
                                verticalAlignment: Qt.AlignVCenter
                                font.bold: true
                                opacity: closeTimePM.checked ? 0.3 : 1
                            }
                            Switch {
                                id: closeTimePM
                                width: closeTimeItem.width / 8
                                height: width / 2
                                anchors.verticalCenter: parent.verticalCenter
                                checked: shadesGroupsModel.selectedCloseAtUS  > 0

                                background: Rectangle {
                                    implicitWidth: closeTimePM.width
                                    implicitHeight: closeTimePM.height/2
                                    y: height / 2
                                    radius: height / 2
                                    color: closeTimePM.checked ? "#000000" : "#ffffff"
                                    border.color: closeTimePM.checked ? "#808080" : "#cccccc"
                                }

                                indicator: Rectangle {
                                    x: closeTimePM.checked ? closeTimePM.width - width : 0
                                    height: closeTimePM.height
                                    width: closeTimePM.height
                                    radius: height / 2
                                    color: closeTimePM.down ? "#cccccc" : "#ffffff"
                                    border.color: closeTimePM.checked ? "#000000" : "#999999"
                                }
                            }
                            Text{
                                text: "PM"
                                height: parent.height
                                font.pixelSize: parent.width / 8
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
                    text: "Show:"
                    visible: showBefore.visible
                }

                ComboBox {
                    id: showBefore
                    width: parent.width
                    height: 40
                    model: shadesGroupsModel.positionOrder
                    visible: shadesGroupsModel.count > 1
                    currentIndex: shadesGroupsModel.selectedGroupPosition
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
                            shadesGroupsModel.removeShadeGroup(controllerMac, selectedChannel);
                            menuClicked();
                        }
                    }
                }
            }
        }

    }

}
