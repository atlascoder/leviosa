import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import com.atlascoder.LocationModel 1.0
import com.atlascoder.TimeZoneModel 1.0

import "DefaultTheme.js" as DefTheme

LeviosaPage {
    id: locationPage

    property alias uuid : locationModel.uuid
    property alias bssid : locationModel.bssid

    property int utcOffset

    signal locationChanged()
    signal locationDeleted()
    signal locationCreated(string uuid)

    enableMenuAction: false
    enableAddAction: false

    title : locationModel.isNew ? "New Location" : ("Edit " + locationModel.name)

    LocationModel {
        id: locationModel
        onUuidChanged: timezoneModel.currentOffset = utcOffset
    }

    TimeZoneModel {
        id: timezoneModel
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
            text: locationModel.name
        }

        Text {
            text: "Time zone:"
        }

        ComboBox {
            id: timezoneCB
            width: parent.width
            height: 40
            model: timezoneModel
            textRole: "signature"
            currentIndex: timezoneModel.selectedIndex
            onCurrentIndexChanged: timezoneModel.selectedIndex = currentIndex
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
        }

        Text {
            text: "Show:"
            visible: showBefore.visible
        }

        ComboBox {
            id: showBefore
            width: parent.width
            height: 40
            model: locationModel.positionOrder
            visible: model.length > 1
            currentIndex: locationModel.position
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

        DeleteButton {
            width: parent.width / 3
            anchors.horizontalCenter: parent.horizontalCenter
            visible: !locationModel.isNew
            onPressAndHold: {
                locationModel.deleteLocation()
                locationDeleted()
            }
        }

    }

    footer: Item {
        width: parent.width
        height: 64

        ActionButton {
            id: cancelBtn
            text: "Cancel"
            outline: true
            anchors.top: parent.top
            anchors.right: parent.horizontalCenter
            anchors.left: parent.left
            anchors.margins: 12
            height: 40
            onClicked: goBack()
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
                locationModel.name = locationName.text
                locationModel.position = showBefore.currentIndex
                locationModel.utcOffset = timezoneModel.utcOffset
                locationModel.saveChanges()
                if (locationModel.isNew)
                    locationCreated(locationModel.uuid)
                else
                    locationChanged()
            }
        }
    }
}
