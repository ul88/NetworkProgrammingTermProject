import QtQuick

Rectangle {
    id: root
    width: 54
    height: 100
    color: "yellow"
    radius: 10
    property string path
    property int imageWidth
    property int imageHeight
    Image{
        width: root.imageWidth
        height: root.imageHeight
        anchors.horizontalCenter: parent.horizontalCenter
        y:13
        fillMode: Image.PreserveAspectFit
        source: root.path
    }
    Rectangle{
        y: 70
        width: 27
        height: 10
        anchors.horizontalCenter: parent.horizontalCenter
    }

}
