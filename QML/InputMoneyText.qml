import QtQuick

Rectangle {
    id: rectangle
    width: 120
    height: 35
    color: "#000000"

    Text{
        color: "#ff0000"
        text: VendingMachine.nowInputMoney
        anchors.centerIn: parent
        font.pointSize: 10
    }
}
