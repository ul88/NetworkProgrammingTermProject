import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 100
    height: 100
    color: "#00ff4e"

    signal successLogin()

    function login(password){
        if(Manager.login(password)){
            loginDialog.close()
            root.successLogin()
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            loginDialog.open()
        }
    }

    Dialog{
        id: loginDialog
        width:450
        height:100
        modal: true
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose
        header: Rectangle {
            height: 40
            width: parent.width - 20
            color: loginDialog.palette.window

            Text {
                text: "관리자 모드"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 12
                font.bold: true
            }

            Button {
                text: "X"
                width: 36
                height: 30
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter

                onClicked: loginDialog.close()
            }
        }

        background: Rectangle {
            border.width: 0
            color: loginDialog.palette.window
        }

        contentItem: RowLayout{
            width: 300
            height: 100
            spacing: 3
            Text{
                verticalAlignment: TextInput.AlignVCenter
                Layout.alignment: Qt.AlignVCenter
                id: text
                text: "비밀번호 :"
            }
            Rectangle{
                id: rectangle
                width: 300
                height: text.height + 10
                border.color: "black"
                border.width: 1
                Layout.alignment: Qt.AlignVCenter
                TextInput{
                    id: textInput
                    anchors.fill: parent
                    echoMode: TextInput.Password

                    onAccepted: {
                        login(textInput.text)
                    }
                }
            }
            Button{
                width: 50
                height: text.height + 10
                text: "로그인"
                Layout.alignment: Qt.AlignVCenter

                onClicked: {
                    login(textInput.text)
                }
            }
        }
    }
}
