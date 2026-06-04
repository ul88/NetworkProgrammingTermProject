import QtQuick
import vending_machine
Item {
    id: root
    width: 480
    height: 25
    required property Window mainWindow
    required property real mouseX
    required property real mouseY
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
                property var money: Consumer.get(index)
                Text {
                    id: label
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 5

                    text: money.cost + "원 : "
                }

                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    rightPadding: 5

                    text: money.count
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if(money.count <= 0) return
                        if(object !== null && object.isRunningAnimate) return
                        nowWonImagePath = money.imagePath
                        if(object === null){
                            object = moneyComponent.createObject(root.mainWindow, {money : money})
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

            property bool isPaper: nowWonImagePath === Consumer.get(0).imagePath ? true : false
            property var money: null
            property bool isRunningAnimate: false
            property alias imageObj: image

            function deleteObject(){
                Consumer.spendMoney(money)
                VendingMachine.addConsumerMoney(money.cost)
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
