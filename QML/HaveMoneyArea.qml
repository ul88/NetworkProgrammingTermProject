import QtQuick

Item {
    id: root
    width: 480
    height: 25
    required property Window mainWindow
    required property real mouseX
    required property real mouseY
    required property var won
    property var wonType: [1000, 500, 100, 50, 10]
    property var wonImagePath: ["qrc:/images/1000won.png",
                                "qrc:/images/500won.png",
                                "qrc:/images/100won.png",
                                "qrc:/images/50won.png",
                                "qrc:/images/10won.png"]
    property var object: null
    property string nowWonImagePath

    function pad5(value) {
        return ("     " + value).slice(-5)
    }

    function moneyText(i) {
        return wonType[i] + "원 : " + pad5(won[i])
    }

    Row{
        property int blockWidth: 90
        leftPadding: 5
        anchors.fill: parent
        spacing: 5
        Repeater{
            model: 5
            Rectangle {
                width: parent.blockWidth
                height: parent.height

                Text {
                    id: label
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 5

                    text: wonType[index] + "원 : "
                }

                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    rightPadding: 5

                    text: won[index]
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if(won[index] <= 0) return
                        if(object !== null && object.isRunningAnimate) return
                        nowWonImagePath = wonImagePath[index]
                        if(object === null){
                            object = moneyComponent.createObject(root.mainWindow, {won : won[index]})
                        }
                    }
                }
            }
        }
    }

    Component{
        id: moneyComponent
        Item{
            id: imageBackground
            width: 100
            height: 100
            clip: true

            property bool isPaper: nowWonImagePath == "qrc:/images/1000won.png" ? true : false
            property int won: 0
            property bool isRunningAnimate: false
            property alias imageObj: image

            function deleteObject(){
                destroy()
            }

            Binding on x{
                when: !isRunningAnimate
                value: root.mouseX - image.width / 2
            }
            Binding on y{
                when: !isRunningAnimate
                value: root.mouseY - image.height / 2
            }

            Image{
                id: image
                width: 100
                height: 100
                source: nowWonImagePath
                fillMode: Image.PreserveAspectFit
            }
        }

    }
}
