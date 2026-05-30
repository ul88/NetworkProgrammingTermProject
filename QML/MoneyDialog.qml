import QtQuick
import QtQuick.Controls

Dialog{
    id: root

    required property string moneyType

    modal: true
    title: moneyType + " 삽입 여부"
    anchors.centerIn: Overlay.overlay
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 300
    height: 150

    contentItem: Text{
        text: moneyType + "을 삽입하시겠습니까?"
    }
}
