import QtQuick
import vending_machine
import QtQuick.Controls
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

    onEnabledChanged: {
        if(enabled === false){
            if(object !== null) object.destroy()
        }
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
                        if(object !== null){
                            object.destroy()
                        }
                        object = moneyComponent.createObject(root.mainWindow, {money : money})
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

            property var money: null
            property bool isRunningAnimate: false
            property alias imageObj: image
            property bool isPaper: money.imagePath === Consumer.get(0).imagePath ? true : false

            function deleteObject(){
                Consumer.spendMoney(money)
                let ret = VendingMachine.addConsumerMoney(money.cost)
                switch(ret){
                case VendingMachine.FAIL_7000:
                    dialog.text = "7000원 이상 금액은 투입할 수 없습니다."
                    dialog.open()
                    break
                case VendingMachine.FAIL_5000:
                    dialog.text = "지폐는 5장까지만 투입 가능합니다."
                    dialog.open()
                    break
                }

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
                source: imageBackground.money.imagePath
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Dialog{
        id: dialog

        property string text: ""
        closePolicy: Popup.NoAutoClose
        modal: true
        title: "투입 실패"
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok
        width: 300
        height: 150

        contentItem: Text{
            text: dialog.text
        }
    }
}
