import QtQuick 2.0

QtObject {

    property ListModel model : ListModel {
        ListElement {
            signature: "EST"
            name: "Eastern"
            utcOffset: -18000
        }
        ListElement {
            signature: "CST"
            name: "Central"
            utcOffset: -21600
        }
        ListElement {
            signature: "MST"
            name: "Mountain"
            utcOffset: -25200
        }
        ListElement {
            signature: "PST"
            name: "Pacific"
            utcOffset: -28800
        }
        ListElement {
            signature: "AKST"
            name: "Alaska"
            utcOffset: -32400
        }
        ListElement {
            signature: "HST"
            name: "Hawaii"
            utcOffset: -36000
        }
    }

    function getSignature(offset){
        var i = getIndex(offset);
        if(i>=0)
            return model.get(i).signature;
        else
            return "UTC" + Math.floor(offset/3600) + (Math.floor(offset % 3600)===0 ? "" : ":" + Math.floor(offset % 3600))
    }

    function getIndex(offset){
        var i = 0;
        while(model.get(i)){
            if(model.get(i).utcOffset === offset){
                return i;
            }
            i++;
        }
        return -1;
    }

}
