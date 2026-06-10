import QtQuick
import vending_machine

Window {
    id: root
    width: 960
    height: 760
    visible: false
    title: "매출확인"
    modality: Qt.ApplicationModal

    property var logFileList: []
    property var currentTable: null
    property string currentViewTitle: "로그 조회"

    function clearTable(){
        if(currentTable !== null){
            currentTable.destroy()
            currentTable = null
        }
    }

    function openLogFile(logName){
        clearTable()
        currentViewTitle = "로그 조회"

        const headers = VendingMachine.getCsvHeader(logName)
        const sortHeader = headers.length > 0 ? headers[0] : ""

        currentTable = managerTableViewComponent.createObject(tableContent, {
            logName: logName,
            headerList: headers,
            logList: VendingMachine.getLogList(logName, sortHeader, VendingMachine.DESC)
        })

        currentTable.anchors.fill = tableContent
        logFileArea.visible = false
        tableHost.visible = true
    }

    function openRevenuePeriodView(){
        clearTable()
        currentViewTitle = "일별/월별 매출"

        currentTable = revenuePeriodViewComponent.createObject(tableContent)
        currentTable.anchors.fill = tableContent
        logFileArea.visible = false
        tableHost.visible = true
    }

    function showLogFileList(){
        clearTable()
        logFileList = VendingMachine.getLogFileList()
        logFileArea.visible = true
        tableHost.visible = false
    }

    onVisibleChanged: {
        if(visible){
            showLogFileList()
        }else{
            clearTable()
        }
    }

    Item{
        id: logFileArea
        anchors.fill: parent
        anchors.margins: 16

        Column{
            anchors.fill: parent
            spacing: 18

            Flow{
                id: logFileFlow
                width: parent.width
                spacing: 12

                Repeater{
                    model: root.logFileList

                    Rectangle{
                        width: 104
                        height: 84
                        radius: 4
                        color: fileMouseArea.containsMouse ? "#efe5d2" : "#f7f3ea"
                        border.color: "#b8aa92"
                        border.width: 1

                        Rectangle{
                            width: 22
                            height: 22
                            anchors.top: parent.top
                            anchors.right: parent.right
                            color: "#ded4c2"
                            border.color: "#b8aa92"
                            border.width: 1
                        }

                        Text{
                            anchors.centerIn: parent
                            width: parent.width - 14
                            text: modelData
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.Wrap
                            elide: Text.ElideRight
                            font.pixelSize: 12
                            font.bold: true
                            color: "#222222"
                        }

                        MouseArea{
                            id: fileMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                root.openLogFile(modelData)
                            }
                        }
                    }
                }
            }

            Row{
                width: parent.width
                spacing: 14

                Repeater{
                    model: ["일별/월별 매출", "재고 소진 날짜 주기"]

                    Rectangle{
                        width: (parent.width - parent.spacing) / 2
                        height: 112
                        radius: 6
                        color: specialFolderMouseArea.containsMouse ? "#e8eef8" : "#f3f6fb"
                        border.color: "#8fa5c2"
                        border.width: 1

                        Rectangle{
                            width: 62
                            height: 18
                            x: 12
                            y: -1
                            radius: 4
                            color: "#d7e2f1"
                            border.color: "#8fa5c2"
                            border.width: 1
                        }

                        Rectangle{
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: parent.height - 14
                            radius: 6
                            color: "transparent"
                            border.color: "transparent"

                            Text{
                                anchors.centerIn: parent
                                width: parent.width - 22
                                text: modelData
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                wrapMode: Text.Wrap
                                font.pixelSize: 14
                                font.bold: true
                                color: "#1f344f"
                            }
                        }

                        MouseArea{
                            id: specialFolderMouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                if(modelData === "일별/월별 매출"){
                                    root.openRevenuePeriodView()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item{
        id: tableHost
        anchors.fill: parent
        visible: false

        Rectangle{
            id: tableHeader
            width: parent.width
            height: 44
            color: "#f7f3ea"
            border.color: "#b8aa92"
            border.width: 1

            Text{
                anchors.centerIn: parent
                text: root.currentViewTitle
                font.pixelSize: 15
                font.bold: true
                color: "#222222"
            }

            Rectangle{
                width: 82
                height: 28
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                radius: 4
                color: backMouseArea.containsMouse ? "#e6dac6" : "#ffffff"
                border.color: "#b8aa92"
                border.width: 1

                Text{
                    anchors.centerIn: parent
                    text: "뒤로가기"
                    font.pixelSize: 12
                    color: "#222222"
                }

                MouseArea{
                    id: backMouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        root.showLogFileList()
                    }
                }
            }
        }

        Item{
            id: tableContent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: tableHeader.bottom
            anchors.bottom: parent.bottom
        }
    }

    Component{
        id: managerTableViewComponent

        ManagerTableView{
        }
    }

    Component{
        id: revenuePeriodViewComponent

        RevenuePeriodView{
        }
    }
}
