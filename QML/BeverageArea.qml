import QtQuick
import QtQuick.Controls
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
                let ret = VendingMachine.buyBeverage(beverage)

                switch(ret){
                case VendingMachine.SUCCESS:
                    dialogStart(beverage.name + "음료 구매에 성공했습니다.")
                    break
                case VendingMachine.FAIL_EMPTY:
                    dialogStart("돈을 투입하지 않아,\n"
                                + beverage.name + "음료 구매에 실패하였습니다.")
                    break
                case VendingMachine.FAIL_SOLD_OUT:
                    dialogStart("자판기에 " + beverage.name + "음료가 존재하지 않습니다.")
                    break
                case VendingMachine.FAIL_INCORRECT_COST:
                    dialogStart(beverage.name + "음료의 가격 " + beverage.cost + "에 맞지 않는 금액을\n투입하였습니다.\n"
                                + "투입한 금액 그대로 반환합니다.")
                    break
                default:
                    break
                }
            }
        }
    }

    function dialogStart(msg){
        dialog.msg = msg
        dialog.open()
    }

    Dialog{
        id: dialog
        closePolicy: Popup.NoAutoClose
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok
        width: 300
        height: 250
        title: "음료 구매 결과"

        property string msg: ""

        contentItem: Text{
            text: dialog.msg
        }
    }

}
