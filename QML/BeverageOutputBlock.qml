import QtQuick

Rectangle {
    width: 440
    height: 100
    color: "#000000"

    Image{
        width: 80
        height: 80
        visible: VendingMachine.topOutputName !== ""
        anchors.centerIn: parent
        source: VendingMachine.topOutputImagePath

        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!parent.visible) return

                VendingMachine.removeOutputBeverage();
            }
        }
    }
}
