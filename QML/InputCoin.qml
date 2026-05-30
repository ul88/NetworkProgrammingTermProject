import QtQuick
import QtQuick.Controls


Rectangle {
    id: root
    width: 50
    height: 50
    color: "#100000"
    radius: width / 2

    property var moneyObj: null

    signal enteredMoney(var moneyObj)

    onEnteredMoney: function(moneyObj){
        if(moneyObj.isPaper) return
        root.moneyObj = moneyObj
        moneyDialog.open()
    }

    function animationStart(){
        root.moneyObj.isRunningAnimate = true
        root.moneyObj.x = root.x - 9
        root.moneyObj.y = root.y - root.height / 2
        animator.animator1Obj = root.moneyObj
        animator.animator2Obj = root.moneyObj.imageObj
        animator.start()
    }

    function deleteMoneyObj(){
        root.moneyObj.deleteObject()
        root.moneyObj = null
    }

    Rectangle{
        width: 4
        height: parent.height - 20
        anchors.centerIn: parent
    }

    MoneyDialog{
        id: moneyDialog
        moneyType: "동전"
        onAccepted: {
            animationStart()
        }
    }


    SequentialAnimation{
        id: animator

        property var animator1Obj: null
        property var animator2Obj: null

        ScaleAnimator{
            id: animator1
            target: animator.animator1Obj
            from: 1
            to: 0.3
            duration: 1000
        }

        XAnimator{
            id: animator2
            target: animator.animator2Obj
            from: 0
            to: -100
            duration: 1000
        }

        onStopped: {
            deleteMoneyObj()
        }
    }
}
