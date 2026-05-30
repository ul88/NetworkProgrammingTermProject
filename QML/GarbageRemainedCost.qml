import QtQuick

Rectangle {
    id: rectangle
    width: 120
    height: 35
    color: "#000000"
    property int remainedMoney: 10000

    Text{
        color: "#ff0000"
        text: remainedMoney
        anchors.centerIn: parent
        font.pointSize: 10
    }
}
