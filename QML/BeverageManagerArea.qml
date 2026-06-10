import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 54
    height: 100
    color: "yellow"
    radius: 10
    property int imageWidth
    property int imageHeight
    property var beverage

    signal modifyBeverage()

    function openDialog(type){
        switch(type){
        case 0:
            dialog0.open()
            break
        case 1:
            dialog1.open()
            break
        case 2:
            dialog2.open()
            break
        case 3:
            dialog3.open()
            break
        }
    }

    Image{
        id:image
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
        Text{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            text: root.beverage ? root.beverage.cost : ""
        }
    }

    MouseArea{
        anchors.fill: parent

        acceptedButtons: Qt.RightButton

        onClicked: (mouse)=>{
            if(mouse.button === Qt.RightButton){
                if(beverage) {
                    beverageMenu.open()
                }else{
                    beverageNullMenu.open()
                }
            }
        }

        Menu{
            id: beverageMenu
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            y: image.height

            Item{
                width: beverageMenu.width
                height: 26

                Text{
                    anchors.left: parent.left
                    leftPadding: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.beverage ? root.beverage.name : "이름없음"
                    font.bold: true
                }

                Text{
                    anchors.right: parent.right
                    rightPadding: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.beverage ? root.beverage.count + "개": ""
                    font.bold: true
                    color: "#555555"
                }
            }

            MenuItem{
                text: "위치 변경하기"
                onTriggered: {
                    openDialog(0)
                }
            }
            MenuItem{
                text: "이름 변경하기"
                onTriggered: {
                    openDialog(2)
                }
            }
            MenuItem{
                text: "개수 추가하기"
                onTriggered: {
                    openDialog(3)
                }
            }
            MenuItem{
                text: "삭제하기"
                onTriggered: {
                    VendingMachine.removeBeverage(root.beverage)
                    modifyBeverage()
                }
            }
        }

        Menu{
            id: beverageNullMenu
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            y: image.height

            Text{
                text: "지정된 음료가 없음"
                font.bold: true
            }

            MenuItem{
                text: "음료 추가하기"
                onTriggered: {
                    openDialog(4)
                }
            }
        }
    }

    Dialog{
        id: dialog0
        closePolicy: Popup.NoAutoClose
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 300
        height: 300

        clip:true

        property int selectedIndex: root.beverage ? root.beverage.index : 0

        onOpened: {
            selectedIndex = root.beverage ? root.beverage.index : 0
            positionListFrame.visible = false
        }

        contentItem: Item {
            anchors.centerIn: parent
            implicitWidth : dialog0.availableWidth
            implicitHeight: dialog0.availableHeight
            Column{
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                spacing: 10
                Text{
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "위치 선택"
                }
                Rectangle{
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 100
                    height: 36
                    radius: 4
                    color: positionListFrame.visible ? "#eeeeee" : "white"
                    border.color: "black"
                    border.width: 1

                    Text{
                        anchors.centerIn: parent
                        text: dialog0.selectedIndex + 1
                        font.bold: true
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            positionListFrame.visible = !positionListFrame.visible
                            if(positionListFrame.visible){
                                Qt.callLater(function(){
                                    positionView.positionViewAtIndex(dialog0.selectedIndex, ListView.Center)
                                })
                            }
                        }
                    }
                }
                Frame{
                    id: positionListFrame
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 100
                    height: 150
                    visible: false
                    clip:true
                    padding: 0

                    ListView{
                        id: positionView
                        anchors.fill: parent
                        model: 21
                        currentIndex: dialog0.selectedIndex
                        boundsBehavior: Flickable.StopAtBounds
                        highlightFollowsCurrentItem: false
                        interactive: true

                        delegate: Rectangle{
                            width: positionView.width
                            height: 30
                            color: dialog0.selectedIndex === index ? "#dcecff" : "white"
                            border.color: "#dddddd"
                            border.width: 1

                            Text{
                                anchors.centerIn: parent
                                text: index + 1
                                font.bold: dialog0.selectedIndex === index
                            }

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    dialog0.selectedIndex = index
                                    positionListFrame.visible = false
                                }
                            }
                        }

                        WheelHandler{
                            target: positionView
                            onWheel: (event) => {
                                const maxY = Math.max(0, positionView.contentHeight - positionView.height)
                                positionView.contentY = Math.max(0, Math.min(maxY, positionView.contentY - event.angleDelta.y / 4))
                                event.accepted = true
                            }
                        }
                    }
                }
            }
        }

        onAccepted: {
            VendingMachine.changeBeverageIndex(root.beverage, dialog0.selectedIndex)
            modifyBeverage()
        }
    }

    Dialog{
        id: dialog2
        closePolicy: Popup.NoAutoClose
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 320
        height: 190

        clip:true

        contentItem: Item{
            implicitWidth: dialog2.availableWidth
            implicitHeight: dialog2.availableHeight

            Column{
                anchors.fill: parent
                anchors.margins: 18
                spacing: 10

                Text{
                    text: "이름 변경"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#222222"
                }

                Text{
                    text: "변경할 음료 이름을 입력하세요."
                    font.pixelSize: 12
                    color: "#666666"
                }

                Rectangle{
                    width: parent.width
                    height: 42
                    radius: 6
                    color: "#ffffff"
                    border.width: 1
                    border.color: nameInput.text.length >= nameInput.maximumLength ? "#d9534f" : "#9aa4b2"

                    Text{
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        text: "최대 16글자"
                        color: "#999999"
                        visible: nameInput.text.length === 0
                        font.pixelSize: 14
                    }

                    TextInput{
                        id: nameInput
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        text: root.beverage ? root.beverage.name : ""
                        maximumLength: 16
                        selectByMouse: true
                        clip: true
                        verticalAlignment: TextInput.AlignVCenter
                        font.pixelSize: 15
                        color: "#222222"
                    }
                }

                Row{
                    width: parent.width

                    Text{
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        text: nameInput.text.length + " / " + nameInput.maximumLength
                        color: nameInput.text.length >= nameInput.maximumLength ? "#d9534f" : "#777777"
                        font.pixelSize: 12
                    }
                }
            }
        }

        onAccepted: {
            VendingMachine.changeBeverageName(root.beverage, nameInput.text)
            modifyBeverage()
        }
    }

    Dialog{
        id: dialog3
        closePolicy: Popup.NoAutoClose
        modal: true
        anchors.centerIn: Overlay.overlay
        standardButtons: Dialog.Ok | Dialog.Cancel
        width: 320
        height: 190

        clip:true

        contentItem: Item{
            implicitWidth: dialog3.availableWidth
            implicitHeight: dialog3.availableHeight

            Column{
                anchors.fill: parent
                anchors.margins: 18
                spacing: 10

                Text{
                    text: "개수 추가"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#222222"
                }

                Text{
                    text: "추가할 개수를 입력하세요."
                    font.pixelSize: 12
                    color: "#666666"
                }

                Rectangle{
                    width: parent.width
                    height: 42
                    radius: 6
                    color: "#ffffff"
                    border.width: 1
                    border.color: countInput.acceptableInput || countInput.text.length === 0 ? "#9aa4b2" : "#d9534f"

                    Text{
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        text: "1~999 사이의 숫자"
                        color: "#999999"
                        visible: countInput.text.length === 0
                        font.pixelSize: 14
                    }

                    TextInput{
                        id: countInput
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        inputMethodHints: Qt.ImhDigitsOnly
                        maximumLength: 3
                        validator: RegularExpressionValidator{
                            regularExpression: /[1-9][0-9]{0,2}/
                        }
                        selectByMouse: true
                        clip: true
                        verticalAlignment: TextInput.AlignVCenter
                        font.pixelSize: 15
                        color: "#222222"
                    }
                }

                Text{
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                    text: countInput.acceptableInput || countInput.text.length === 0
                          ? "현재 개수: " + (root.beverage ? root.beverage.count : "0")
                          : "1~999 사이의 숫자를 입력하세요"
                    color: countInput.acceptableInput || countInput.text.length === 0 ? "#777777" : "#d9534f"
                    font.pixelSize: 12
                }
            }
        }

        onAccepted: {
            VendingMachine.changeBeverageCount(root.beverage, Number(countInput.text))
            modifyBeverage()
        }
    }
}
