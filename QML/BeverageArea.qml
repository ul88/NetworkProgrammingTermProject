import QtQuick

Rectangle {
    id: root
    width: 54
    height: 100
    color: "yellow"
    radius: 10
    property int imageWidth
    property int imageHeight
    property var beverage
    Image{
        width: root.imageWidth
        height: root.imageHeight
        anchors.horizontalCenter: parent.horizontalCenter
        y:13
        fillMode: Image.PreserveAspectFit
        source: root.beverage ? root.beverage.imagePath : ""
    }
    Rectangle{
        y: 70
        width: 50
        height: 20
        anchors.horizontalCenter: parent.horizontalCenter
        color : root.beverage ? (root.beverage.count !== 0 ? "green" : "red") : "white"
        Text{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            text: root.beverage ? root.beverage.cost : ""
        }

        MouseArea{
            anchors.fill: parent

            onClicked: {
                if(!beverage) return
                if(root.beverage.count <= 0) return
                VendingMachine.buyBeverage(beverage)
            }
        }
    }

}
