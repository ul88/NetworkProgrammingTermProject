import QtQuick
import QtQuick.Controls
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
    property bool managerState: false

    onManagerStateChanged: {
        if(managerState){
            userComponent.enabled = false
            window.x -= window.width
            window.width *= 2
            changedManagerModeAnim.start()
        }else{
            mangerComponent.enabled = false
            mangerComponent.visible = false
            changedUserModeAnim.start()
        }
    }

    XAnimator{
        id: changedManagerModeAnim
        target: userComponent
        from: 480
        to: 0
        duration: 1000

        onStopped: {
            mangerComponent.enabled = true
            mangerComponent.visible = true
            userComponent.opacity = 0.7
        }
    }

    XAnimator{
        id: changedUserModeAnim
        target: userComponent
        from: 0
        to: 480
        duration: 1000

        onStopped:{
            window.width /=2
            window.x += window.width
            userComponent.x = 0
            userComponent.enabled = true
            userComponent.opacity = 1
        }
    }

    Item{
        id: mangerComponent
        enabled: false
        visible: false
        width:480
        height:640
        x:480

        Button{
            id: button
            width: 30
            height: 20
            text:"X"

            onClicked: {
                managerState = false
            }
            x: 450
        }

        Flow {
            id: managerFlow
            width: parent.width - 20
            anchors.top: parent.top
            anchors.topMargin: 10
            padding: 10
            clip: false
            layer.wrapMode: ShaderEffectSource.ClampToEdge
            anchors.horizontalCenter: parent.horizontalCenter

            function reloadRepeater(){
                managerRepeater.model = 0
                managerRepeater.model = 21
            }

            Repeater {
                id: managerRepeater
                visible: true
                model: 21
                property int modelCount: 15
                z: 1
                delegate: BeverageManagerArea{
                    beverage: VendingMachine.getBeverage(index)
                    imageWidth: 65
                    imageHeight: 65
                    nowIndex: index

                    onModifyBeverage: {
                        modifyButtonRow.visible = true
                        managerFlow.reloadRepeater()
                    }
                }
            }
            spacing: 10
        }

        Row{
            id: modifyButtonRow
            x: 20
            y: 355
            spacing: 10
            visible: false
            property int buttonWidth: 80
            property int buttonHeight: 35

            Button{
                width: modifyButtonRow.buttonWidth
                height: modifyButtonRow.buttonHeight
                text:"수정 완료"

                onClicked: {
                    VendingMachine.jsonUpdateVMBeverage()
                    managerFlow.reloadRepeater()
                    userFlow.reloadRepeater()
                    modifyButtonRow.visible = false
                    moneyRepeater.moneyRepeaterReload()
                }
            }
            Button{
                width: modifyButtonRow.buttonWidth
                height: modifyButtonRow.buttonHeight
                text: "수정 취소"

                onClicked: {
                    VendingMachine.reload() // 기존 파일대로 다시 재설정하기 위함.
                    managerFlow.reloadRepeater()
                    modifyButtonRow.visible = false
                    moneyRepeater.moneyRepeaterReload()
                }
            }

        }

        Row{
            id: managerMoneyRow
            anchors.top: modifyButtonRow.bottom
            anchors.topMargin: 12
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Repeater{
                id: moneyRepeater
                model: 5

                function moneyRepeaterReload(){
                    moneyRepeater.model = 0
                    moneyRepeater.model = 5
                }

                delegate: Rectangle{
                    id: moneyRepeaterRectangle
                    width: 84
                    height: 62
                    radius: 6
                    color: "#f6f1e8"
                    border.color: "#c8bca8"
                    border.width: 1

                    property var money: VendingMachine.getMoney(index)

                    Column{
                        anchors.fill: parent
                        anchors.margins: 6
                        spacing: 4

                        Row{
                            width: parent.width
                            height: 35
                            spacing: 5

                            Image{
                                width: 35
                                height: 35
                                source: moneyRepeaterRectangle.money ? moneyRepeaterRectangle.money.imagePath : ""
                                fillMode: Image.PreserveAspectFit
                            }

                            Text{
                                width: parent.width - 40
                                height: parent.height
                                text: moneyRepeaterRectangle.money? moneyRepeaterRectangle.money.cost + "원" : ""
                                font.pixelSize: 11
                                font.bold: true
                                color: "#222222"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        Text{
                            width: parent.width
                            height: 11
                            text: moneyRepeaterRectangle.money ? moneyRepeaterRectangle.money.count : ""
                            font.pixelSize: 11
                            color: "#555555"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }

        Button{
            id: collectionButton
            anchors.top: managerMoneyRow.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 12
            text: "수금하기"
            width: 440
            height: 30

            onClicked: {
                VendingMachine.collection()
                moneyRepeater.moneyRepeaterReload()
            }
        }

        Button{
            id: salesCheckButton
            anchors.top: collectionButton.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 12
            text: "매출확인하기"
            width: 440
            height: 30

            onClicked: {
                salesCheckWindow.show()
                salesCheckWindow.requestActivate()
            }
        }

        SalesCheckWindow{
            id: salesCheckWindow
            transientParent: window
        }
    }

    Item {
        id: userComponent
        width: 480
        height: 640

        onEnabledChanged: {
            if(enabled){
                userFlow.reloadRepeater()
            }
        }

        Flow {
            id: userFlow
            width: parent.width - 20
            anchors.top: parent.top
            anchors.topMargin: 10
            padding: 10
            clip: false
            layer.wrapMode: ShaderEffectSource.ClampToEdge
            anchors.horizontalCenter: parent.horizontalCenter

            function reloadRepeater(){
                repeater.model = 0
                repeater.model = 21
            }

            Repeater {
                id: repeater
                visible: true
                model: 21
                z: 1
                property int modelCount: 15
                delegate: BeverageArea {
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
            x: userFlow.x + userFlow.anchors.topMargin
            y: userFlow.y + userFlow.height + userFlow.anchors.topMargin
            width: 180
            height: 110
            onSuccessLogin: {
                managerState = true
            }
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

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
            onPositionChanged: function (mouse) {
                window.mouseX = mouse.x
                window.mouseY = mouse.y

                if (haveMoneyArea.object === null)
                    return
                if (haveMoneyArea.object.isRunningAnimate)
                    return
                if (window.mouseX >= inputCoin.x
                        && window.mouseX <= inputCoin.x + inputCoin.width
                        && window.mouseY >= inputCoin.y
                        && window.mouseY <= inputCoin.y + inputCoin.height) {
                    inputCoin.enteredMoney(haveMoneyArea.object)
                }
                if (window.mouseX >= inputPaperMoney.x
                        && window.mouseX <= inputPaperMoney.x + inputPaperMoney.width
                        && window.mouseY >= inputPaperMoney.y
                        && window.mouseY <= inputPaperMoney.y + inputPaperMoney.height) {
                    inputPaperMoney.enteredMoney(haveMoneyArea.object)
                }
            }
        }
    }

}
