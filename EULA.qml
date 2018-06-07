import QtQuick 2.7
import QtQuick.Controls 2.2

LeviosaPage {
    id: eulaPage
    signal agree()
    signal decline()

    title: "Leviosa Motor Shades"

    enableMenuAction: false

    Flickable {
        anchors.fill:parent
        contentWidth: eulaPage.width
        contentHeight: contentItem.childrenRect.height
        Text {
            Component.onCompleted: {
                console.log("Width: " + width)
            }
            width: parent.width
            rightPadding: 8
            leftPadding: 8
            wrapMode: Text.WordWrap
            text: "
<h3>End-User License Agreement of Leviosa Motor Shades</h3>
<br>
This End-User License Agreement (\"EULA\") is a legal agreement between you and Leviosa Motor Shades.
<br>
<br>
This EULA agreement governs your acquisition and use of our Leviosa Motor Shades Application (\"Application\") directly from Leviosa Motor Shades or indirectly through online applications stores (a \"Distributor\").
<br>
<br>
Please read this EULA agreement carefully before completing the installation process and using the Leviosa Motor Shades software. It provides a license to use the Leviosa Motor Shades software and contains warranty information and liability disclaimers.
<br>
<br>
By clicking <b>\"ACCEPT\"</b> or installing and/or using the Leviosa Motor Shades Application, you are confirming your acceptance of the Application and agreeing to become bound by the terms of this EULA agreement.
<br>
<br>
If you are entering into this EULA agreement on behalf of a company or other legal entity, you represent that you have the authority to bind such entity and its affiliates to these terms and conditions. If you do not have such authority or if you do not agree with the terms and conditions of this EULA agreement, do not install or use the Software, and you must not accept this EULA agreement.
<br>
<br>
This EULA agreement shall apply only to the Software supplied by Leviosa Motor Shades herewith regardless of whether other software is referred to or described herein. The terms also apply to any Leviosa Motor Shades updates, supplements, Internet-based services, and support services for the Software, unless other terms accompany those items on delivery. If so, those terms apply.
<br>
<h4>License Grant</h4>
<br>
Leviosa Motor Shades hereby grants you a personal, non-transferable, non-exclusive license to use the Leviosa Motor Shades Application on your devices in accordance with the terms of this EULA agreement.
<br>
<br>
<b>You are permitted to:</b>
<br>
<br>
- Load the Leviosa Motor Shades Application under your control. You are responsible for ensuring your device meets the minimum requirements of the Leviosa Motor Shades Application.
<br>
- Use Corresponding Application Code from public repository at <a href=\"https://github.com/atlascoder/leviosa\">https://github.com/atlascoder/leviosa</a> to build Application with another versions of Qt5 libraries in compliance to terms of LGPLv3.
<br>
<br>
<b>You are not permitted to:</b>
<br>
<br>
- Edit, alter, modify, adapt, translate or otherwise change the Application code nor permit the whole or any part of the Software to be combined with or become incorporated in any other software, excepting rebuilding Application with modified Qt5 libraries
<br>
- Reproduce, copy, distribute, resell or otherwise use the Application for any commercial purpose
<br>
- Allow any third party to use the Application on behalf of or for the benefit of any third party
<br>
- Use the Application in any way which breaches any applicable local, national or international law
<br>
 - Use the Application for any purpose that Leviosa Motor Shades considers a breach of this EULA agreement
<br>
<h4>Intellectual Property and Ownership</h4>
<br>
Leviosa Motor Shades shall at all times retain ownership of the Application as originally downloaded by you and all subsequent downloads of the Software by you. The Software (and the copyright, and other intellectual property rights of whatever nature in the Software, including any modifications made thereto) are and shall remain the property of Leviosa Motor Shades.
<br>
<br>
Application compiled with third-part libraries that are owned by their publishers:
<br>
<br>
- <b>Qt5</b> library (using LGPLv3) that is property of <b>The Qt Company</b>
<br>
- <b>AWS-SDK-CPP</b> library (using Apache License Version 2) that is property of <b>Amazon Web Services, Inc</b>
<br>
- <b>Boost Libraries</b> (using Boost Software License Version 1) that is property of <b>Boost Community</b>
<br>
- <b>Espressif EspTouch</b> library (using ESPRSSIF MIT License) that is property of <b>Epressif Systems</b>
<br>
<h4>Termination</h4>
<br>
This EULA agreement is effective from the date you first use the Software and shall continue perpetual.
<br>
<br>
It will terminate immediately if you fail to comply with any term of this EULA agreement. Upon such termination, the licenses granted by this EULA agreement will immediately terminate and you agree to stop all access and use of the Software. The provisions that by their nature continue and survive will survive any termination of this EULA agreement.
<br>
<br>
This EULA agreement, and any dispute arising out of or in connection with this EULA agreement, shall be governed by and construed in accordance with the laws of United States.
<br>"
        }

    }

    footer: Rectangle {
        width: parent.width
        height: 80
        visible: !currentUser.licenseAgreed
        ActionButton {
            height: parent.width / 12
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: parent.width / 24
            text: "ACCEPT"
            onClicked: {
                currentUser.licenseAgreed = true
                agree()
            }
        }
        ActionButton {
            height: parent.width / 12
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: parent.width / 24
            outline: true
            text: "DECLINE"
            onClicked: decline()
        }
    }
}
