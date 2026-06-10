import QtQuick

Item {
    id: root
    width: 480
    height: 240

    property var logList: []
    property var headerList: []

    function columnWidth(column) {
        if(!headerList || headerList.length === 0)
            return root.width

        return root.width / headerList.length
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

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: 6
                        anchors.rightMargin: 6
                        text: modelData
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Flickable {
            width: parent.width
            height: parent.height - headerRow.height
            contentWidth: width
            contentHeight: logColumn.height
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: logColumn
                width: parent.width
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
