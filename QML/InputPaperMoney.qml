import QtQuick

Rectangle {
    id: root
    width: 110
    height: 25
    color: "#000000"

    property var moneyObj: null

    signal enteredMoney(var moneyObj)

    onEnteredMoney: function(moneyObj){
        if(!moneyObj.isPaper) return
        root.moneyObj = moneyObj
        moneyDialog.open()
    }

    function animationStart(){
        root.moneyObj.isRunningAnimate = true
        root.moneyObj.x = root.x + 3
        root.moneyObj.y = root.y + root.height/2
        animator.animator1Obj = root.moneyObj
        animator.animator2Obj = root.moneyObj.imageObj
        animator.start()
    }

    // 실제 자판기 로직 코드 실행은 이 부분에서 진행
    function deleteMoneyObj(){
        root.moneyObj.deleteObject()
        root.moneyObj = null
    }

    MoneyDialog{
        id: moneyDialog
        moneyType: "지폐"
        onAccepted: {
            animationStart()
        }
    }

    SequentialAnimation{
        id: animator

        property var animator1Obj: null
        property var animator2Obj: null

        RotationAnimator{
            id: animator1
            target: animator.animator1Obj
            from: 0
            to: 90
            duration: 500
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
