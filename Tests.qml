import QtQuick 2.7
import QtQuick.Controls 2.2

import com.atlascoder.CognitoSyncTest 1.0

import "DefaultTheme.js" as DefTheme

WelcomeBrandPage {
    id: rootItem

    hintText: "Testing Screen"

    CognitoSyncTest {
        id: syncTest
    }

    Component.onCompleted: function(){
        syncTest.runTests()
    }

}
