import QtQuick
import QtQuick.Controls
import vending_machine

Item {
    id: root
    width: 480
    height: 240

    property string logName: ""
    property var logList: []
    property var headerList: []
    property string sortHeaderName: headerList && headerList.length > 0 ? headerList[0] : ""
    property int sortType: VendingMachine.DESC
    property int scrollBarWidth: 12
    readonly property real bodyWidth: root.width - scrollBarWidth

    function columnWidth(column) {
        if(!headerList || headerList.length === 0)
            return bodyWidth

        return bodyWidth / headerList.length
    }

    function sortByHeader(headerName) {
        if(logName === "")
            return

        if(sortHeaderName === headerName){
            sortType = sortType === VendingMachine.ASC ? VendingMachine.DESC : VendingMachine.ASC
        }else{
            sortHeaderName = headerName
            sortType = VendingMachine.DESC
        }

        logList = VendingMachine.getLogList(logName, sortHeaderName, sortType)
    }

    Column {
        anchors.fill: parent
        spacing: 0

        Row {
            id: headerRow
            width: parent.width
            height: 30

            Repeater {
                model: root.headerList ? root.headerList : []

                Rectangle {
                    width: root.columnWidth(index)
                    height: headerRow.height
                    border.width: 1
                    color: headerMouseArea.containsMouse ? "#efe5d2" : "#f7f3ea"

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 6
                        anchors.rightMargin: 6
                        text: modelData + (root.sortHeaderName === modelData
                                           ? (root.sortType === VendingMachine.ASC ? " ▲" : " ▼")
                                           : "")
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        id: headerMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            root.sortByHeader(modelData)
                        }
                    }
                }
            }
        }

        Flickable {
            id: logFlickable
            width: parent.width
            height: parent.height - headerRow.height
            contentWidth: root.bodyWidth
            contentHeight: logColumn.height
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AlwaysOn
                width: root.scrollBarWidth
            }

            Column {
                id: logColumn
                width: root.bodyWidth
                spacing: 0

                Repeater {
                    model: root.logList ? root.logList : []

                    Row {
                        width: logColumn.width
                        height: 35
                        property var rowData: modelData

                        Repeater {
                            model: root.headerList ? root.headerList : []

                            Rectangle {
                                width: root.columnWidth(index)
                                height: 35
                                border.width: 1

                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 8
                                    text: rowData[modelData] === undefined ? "" : rowData[modelData]
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
