import QtQuick
import vending_machine

Window {
    id: window
    width: 480
    height: 640
    visible: true
    color: "#3c1717"
    title: qsTr("자판기")

    property real mouseX: 0
    property real mouseY: 0

    Flow{
        id: flow1
        width: parent.width - 20
        anchors.top: parent.top
        anchors.topMargin: 10
        padding: 10
        clip: false
        layer.wrapMode: ShaderEffectSource.ClampToEdge
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater {
            id: repeater
            visible: true
            model: 21
            property int modelCount: 15
            z: 1
            delegate: BeverageArea{
                beverage: VendingMachine.getBeverage(index)
                id: beverageArea
                imageWidth: 65
                imageHeight: 65
            }
        }
        spacing: 10
    }

    PromotionBlock {
        id: promotionBlock
        x: flow1.x + flow1.anchors.topMargin
        y: flow1.y + flow1.height + flow1.anchors.topMargin
        width: 180
        height: 110
    }

    InputMoneyText {
        id: inputMoneyText
        x: promotionBlock.x + promotionBlock.width + 10
        y: promotionBlock.y
    }

    InputPaperMoney {
        id: inputPaperMoney
        x: inputMoneyText.x + 5
        y: inputMoneyText.y + inputMoneyText.height + 10
    }

    InputCoin {
        id: inputCoin
        x: 400
        y: promotionBlock.y
    }

    BeverageOutputBlock {
        id: beverageOutputBlock
        x: 20
        y: 505
    }

    HaveMoneyArea {
        id: haveMoneyArea
        x: 0
        y: 610
        mainWindow: window
        mouseX: window.mouseX
        mouseY: window.mouseY
    }

    OutputMoneyArea {
        id: outputMoneyArea
        x: 350
        y: 421
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
        onPositionChanged: function(mouse){
            window.mouseX = mouse.x
            window.mouseY = mouse.y

            if(haveMoneyArea.object === null) return
            if(haveMoneyArea.object.isRunningAnimate) return
            if(window.mouseX >= inputCoin.x && window.mouseX <= inputCoin.x + inputCoin.width &&
                    window.mouseY >= inputCoin.y && window.mouseY <= inputCoin.y + inputCoin.height){
                inputCoin.enteredMoney(haveMoneyArea.object)
            }
            if(window.mouseX >= inputPaperMoney.x && window.mouseX <= inputPaperMoney.x + inputPaperMoney.width &&
                    window.mouseY >= inputPaperMoney.y && window.mouseY <= inputPaperMoney.y + inputPaperMoney.height){
                inputPaperMoney.enteredMoney(haveMoneyArea.object)
            }
        }
    }
}
