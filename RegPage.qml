import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "DefaultTheme.js" as DefTheme
import "libs/aws-sdk.js" as AWS

Page {
	id: registerPage
    visible: true
    width: 480
    height: 800
	title: qsTr("Authentication")

	background: Rectangle {
		anchors.fill: parent
		color: DefTheme.mainColorBg
	}

	signal signedUp()
	signal restore()
	signal authenticate()

	Column {
		id: column
        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
		spacing: 5

		Rectangle {
			width: parent.width * 0.8
			color: "#00000000"
			Image {
				id: logoPic
				width: parent.width
				fillMode: Image.PreserveAspectFit
				source: "img/logo-big.svg"
			}
			ColorOverlay {
				width: logoPic.width
				height: logoPic.height
				source: logoPic
				color: DefTheme.mainColorDark
			}
			height: childrenRect.height
			anchors.horizontalCenter: parent.horizontalCenter
		}

        Rectangle {
			height: registerPage.height * 0.02
            width: parent.width
            color: "#00000000"
        }

        Rectangle {
			height: registerPage.height * 0.08
            width: parent.width

            color: "#00000000"

            Image {
                id: emailIcon
				source: "img/003-black-back-closed-envelope-shape.svg"
                height: parent.height / 2
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                width: height
                x: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: emailInput
                anchors.left: emailIcon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
				height: emailIcon.height
				horizontalAlignment: Text.AlignHCenter
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "enter valid email"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
				focus: true
			}

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: emailInput.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

        }

        Rectangle {
			height: registerPage.height * 0.08
            width: parent.width
            color: "#00000000"

            Image {
                id: passwordIcon
				source: "img/001-write.svg"
                height: parent.height / 2
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                width: height
                x: 10
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: passwordInput
                anchors.left: passwordIcon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
                height: passwordIcon.height
				horizontalAlignment: Text.AlignHCenter
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "enter a password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
			}

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: passwordInput.bottom
                anchors.topMargin: 2
                width: parent.width - 20
                height: 2
                color: "#000000"
            }

        }

		Rectangle {
			height: registerPage.height * 0.08
			width: parent.width
			color: "#00000000"
			border.color: "#00000000"

			Image {
				id: passwordConfirmIcon
				source: "img/001-write.svg"
				height: parent.height / 2
				antialiasing: true
				fillMode: Image.PreserveAspectFit
				width: height
				x: 10
				anchors.verticalCenter: parent.verticalCenter
			}

			TextInput {
				id: passwordConfirmInput
				anchors.left: passwordConfirmIcon.right
				anchors.right: parent.right
				anchors.rightMargin: 10
				height: passwordConfirmIcon.height
				horizontalAlignment: Text.AlignHCenter
				font.capitalization: Font.MixedCase
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: parent.height / 3
				z: 1
				Text {
					anchors.fill: parent
					visible: !(parent.focus || parent.text.length > 0)
					color: "#b1b1b1"
					text: "confirm password"
					verticalAlignment: Text.AlignVCenter
					font.italic: true
					horizontalAlignment: Text.AlignHCenter
				}
			}

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: passwordConfirmInput.bottom
				anchors.topMargin: 2
				width: parent.width - 20
				height: 2
				color: "#000000"
			}

		}

        Rectangle {
			id: signUpButton
			color: DefTheme.mainColorLight
			border.color: DefTheme.mainColorLight
            width: parent.width
			height: registerPage.height * 0.08
            radius: height * 0.4
            Text {
				color: DefTheme.mainTextColor
                anchors.centerIn: parent
                font.pixelSize: parent.height / 2
				text: qsTr("SIGN UP")
            }
			function signUp(email, pwd){

				var poolData = {
					UserPoolId : 'us-east-2_upLJUtE0q',
					ClientId : '1bilbv38m3hrogdkmmdfe9ufq5'
				};
				console.log(DefTheme.AWS.toString());
				console.log(AWS.Cognito.CognitoIdentityServiceProvider.toString());
				console.log(AWS.AWS.CognitoIdentityServiceProvider.CognitoUserPool);

				var userPool = new AWS.CognitoIdentityServiceProvider.CognitoUserPool(poolData);

				var attributeList = [];

				var dataEmail = {
					Name : 'email',
					Value : email
				};

				var attributeEmail = new AWS.CognitoIdentityServiceProvider.CognitoUserAttribute(dataEmail);

				attributeList.push(attributeEmail);

				var reg_result;

				userPool.signUp('username', 'password', attributeList, null, function(err, result){
					if (err) {
						alert(err);
						return;
					}
					cognitoUser = result.user;
					reg_result = result;
					console.log('user name is ' + cognitoUser.getUsername());
				});
				return reg_result;
			}
			MouseArea {
				anchors.fill: parent
				onClicked: {
					if(signUpButton.signUp(emailInput.text, passwordInput.text)){
						signedUp();
						console.log("SignUp success");
					}
					else{
						console.log("SignUp failed");
					}
				}
			}
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: registerPage.height * 0.03
        }

        Rectangle {
            color: "#00000000"
            width: parent.width
			height: registerPage.height * 0.05

            Rectangle {
                id: restoreButton
                anchors.right: parent.right
				color: DefTheme.secColor
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Restore"
					font.pointSize: 14
					font.family: "Verdana"
					topPadding: 3
					bottomPadding: 3
					leftPadding: 6
					rightPadding: 6
					color: DefTheme.mainTextColor
					font.capitalization: Font.AllUppercase
				}
				width: childrenRect.width
				height: childrenRect.height
                radius: height * 0.5

				MouseArea {
					anchors.fill: parent
					onClicked: restore()
				}
            }

            Text {
                anchors.right: restoreButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Forgot password?")
            }
        }

        Rectangle {
            id: rectangle
            color: "#00000000"
            width: parent.width
			height: registerPage.height * 0.05

            Rectangle {
                id: registerButton
                anchors.right: parent.right
				color: DefTheme.secColor
                anchors.verticalCenter: parent.verticalCenter
                Text{
					text: "Sign in"
					font.pointSize: 14
					style: Text.Normal
					topPadding: 3
					bottomPadding: 3
					leftPadding: 6
					rightPadding: 6
					color: DefTheme.mainTextColor
					font.capitalization: Font.AllUppercase
				}
				width: childrenRect.width
				height: childrenRect.height
                radius: height * 0.5

				MouseArea {
					anchors.fill: parent
					onClicked: authenticate()
				}
            }

            Text {
                anchors.right: registerButton.left
				anchors.rightMargin: 8
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Have account already?")
			}
        }
    }


}
