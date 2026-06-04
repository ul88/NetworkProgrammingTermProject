import QtQuick
import QtQuick.Controls
Rectangle {
    width: 100
    height: 70
    color: "#000000"

    clip: true

    function openDialog(){
        let returnMoneyList = VendingMachine.getReturnMoneyList();
        let text = "";
        for(let i = 0; i < returnMoneyList.length;i++){
            text += returnMoneyList[i].cost + "원은 " + returnMoneyList[i].count + "개 반환됩니다.\n";
        }

        returnDialog.text = text
        returnDialog.open()
    }

    Item{
        id: item
        anchors.centerIn: parent
        width: 90
        height: 60
        rotation: 15

        visible: VendingMachine.isReturnMoney

        Image{
            anchors.fill: parent
            source: "qrc:/images/money_group"
        }
    }

    MouseArea{
        anchors.fill: item
        onClicked: {
            if(!item.visible) return
            openDialog()
        }
    }

    Dialog{
        id: returnDialog

        property string text

        modal: true
        title: "수거되는 돈"
        anchors.centerIn: Overlay.overlay
        standardButtons: text !== "" ? Dialog.Ok | Dialog.Cancel : Dialog.Cancel
        width: 300
        height: 200
        contentItem: Text{
            text: returnDialog.text
        }

        onAccepted: {
            VendingMachine.returnMoney(Consumer);
        }
    }
}
