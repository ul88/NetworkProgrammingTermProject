import QtQuick
import QtQuick.Controls
import vending_machine

Item {
    id: root

    property var revenueList: []
    property var resultList: []
    property var graphData: []
    property var yearList: []
    property var monthList: ["모든 날", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"]
    property var dayList: ["모든 날", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
                           "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                           "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"]
    property bool graphMode: false
    property int scrollBarWidth: 12
    property real bodyWidth: Math.max(0, width - scrollBarWidth)
    property int graphMaxValue: 0
    property int totalRevenue: 0
    readonly property var tableHeaders: [
        { "title": "날짜", "role": "date" },
        { "title": "시간", "role": "time" },
        { "title": "오전/오후", "role": "ampm" },
        { "title": "음료", "role": "beverage" },
        { "title": "가격", "role": "priceText" }
    ]

    function formatNumber(value){
        var text = String(Math.round(Number(value) || 0))
        return text.replace(/\B(?=(\d{3})+(?!\d))/g, ",")
    }

    function columnWidth(column){
        var weights = [0.24, 0.18, 0.14, 0.26, 0.18]
        return bodyWidth * weights[column]
    }

    function normalizeRow(row){
        var dateText = row["날짜"] === undefined ? "" : String(row["날짜"])
        if(dateText.length < 10)
            return null

        var timeText = dateText.length >= 19 ? dateText.substring(11, 19) : ""
        var hour = Number(timeText.substring(0, 2))
        var priceText = row["음료가격"] === undefined ? "0" : String(row["음료가격"])
        var price = Number(priceText.replace(/,/g, ""))
        if(isNaN(price))
            price = 0

        return {
            "date": dateText.substring(0, 10),
            "time": timeText,
            "ampm": isNaN(hour) ? "" : (hour < 12 ? "오전" : "오후"),
            "beverage": row["음료명"] === undefined ? "" : String(row["음료명"]),
            "price": price,
            "priceText": formatNumber(price),
            "year": dateText.substring(0, 4),
            "month": dateText.substring(5, 7),
            "day": dateText.substring(8, 10)
        }
    }

    function loadData(){
        var rows = VendingMachine.getLogList("log", "날짜", VendingMachine.DESC)
        var normalized = []
        var years = []
        var seenYears = {}

        for(var i = 0; i < rows.length; i++){
            var revenue = normalizeRow(rows[i])
            if(revenue === null)
                continue

            normalized.push(revenue)
            if(!seenYears[revenue.year]){
                years.push(revenue.year)
                seenYears[revenue.year] = true
            }
        }

        years.sort()
        if(years.length === 0)
            years.push(String(new Date().getFullYear()))

        revenueList = normalized
        yearList = years
        Qt.callLater(search)
    }

    function search(){
        var year = yearBox.currentText
        var month = monthBox.currentText
        var day = dayBox.currentText
        var rows = []
        var sum = 0

        for(var i = 0; i < revenueList.length; i++){
            var revenue = revenueList[i]
            var matched = revenue.year === year
            if(month !== "모든 날")
                matched = matched && revenue.month === month

            if(day !== "모든 날")
                matched = matched && revenue.day === day

            if(matched){
                rows.push(revenue)
                sum += revenue.price
            }
        }

        resultList = rows
        totalRevenue = sum
        graphData = makeGraphData(rows)
    }

    function makeGraphData(rows){
        var monthAll = monthBox.currentText === "모든 날"
        var dayAll = dayBox.currentText === "모든 날"
        var map = {}
        var labels = []
        var maxValue = 0

        for(var i = 0; i < rows.length; i++){
            var key = rows[i].beverage
            if(monthAll){
                key = rows[i].month
            }else if(dayAll){
                key = rows[i].day
            }

            if(!map[key]){
                map[key] = 0
                labels.push(key)
            }

            map[key] += rows[i].price
            maxValue = Math.max(maxValue, map[key])
        }

        labels.sort(function(left, right){
            if(monthAll || dayAll)
                return Number(left) - Number(right)

            return String(left).localeCompare(String(right))
        })

        var data = []
        for(var j = 0; j < labels.length; j++){
            var label = labels[j]
            if(monthAll){
                label = labels[j] + "월"
            }else if(dayAll){
                label = labels[j] + "일"
            }

            data.push({
                          "label": label,
                          "value": map[labels[j]]
                      })
        }

        graphMaxValue = maxValue
        return data
    }

    Component.onCompleted: loadData()

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Row {
            width: parent.width
            height: 34
            spacing: 8

            ComboBox {
                id: yearBox
                width: 92
                height: parent.height
                model: root.yearList
            }

            ComboBox {
                id: monthBox
                width: 88
                height: parent.height
                model: root.monthList
            }

            ComboBox {
                id: dayBox
                width: 104
                height: parent.height
                model: root.dayList
            }

            Button {
                width: 70
                height: parent.height
                text: "검색"
                onClicked: root.search()
            }

            Button {
                width: 92
                height: parent.height
                text: root.graphMode ? "표 보기" : "그래프 보기"
                onClicked: root.graphMode = !root.graphMode
            }
        }

        Rectangle {
            width: parent.width
            height: 34
            radius: 4
            color: "#f7f3ea"
            border.color: "#b8aa92"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                text: "검색 결과 " + root.resultList.length + "건 / 합계 " + root.formatNumber(root.totalRevenue) + "원"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 13
                font.bold: true
                color: "#222222"
                elide: Text.ElideRight
            }
        }

        Item {
            width: parent.width
            height: parent.height - y

            Column {
                anchors.fill: parent
                visible: !root.graphMode
                spacing: 0

                Row {
                    id: tableHeaderRow
                    width: parent.width
                    height: 32

                    Repeater {
                        model: root.tableHeaders

                        Rectangle {
                            width: root.columnWidth(index)
                            height: tableHeaderRow.height
                            color: "#f7f3ea"
                            border.color: "#b8aa92"
                            border.width: 1

                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 4
                                anchors.rightMargin: 4
                                text: modelData.title
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 12
                                font.bold: true
                                color: "#222222"
                                elide: Text.ElideRight
                            }
                        }
                    }
                }

                Flickable {
                    width: parent.width
                    height: parent.height - tableHeaderRow.height
                    contentWidth: root.bodyWidth
                    contentHeight: tableColumn.height
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AlwaysOn
                        width: root.scrollBarWidth
                    }

                    Column {
                        id: tableColumn
                        width: root.bodyWidth
                        spacing: 0

                        Repeater {
                            model: root.resultList

                            Row {
                                width: tableColumn.width
                                height: 34
                                property var rowData: modelData

                                Repeater {
                                    model: root.tableHeaders

                                    Rectangle {
                                        width: root.columnWidth(index)
                                        height: 34
                                        color: "#ffffff"
                                        border.color: "#d0d0d0"
                                        border.width: 1

                                        Text {
                                            anchors.fill: parent
                                            anchors.leftMargin: 6
                                            anchors.rightMargin: 6
                                            text: rowData[modelData.role]
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            font.pixelSize: 12
                                            color: "#222222"
                                            elide: Text.ElideRight
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Flickable {
                anchors.fill: parent
                visible: root.graphMode
                contentWidth: width
                contentHeight: graphColumn.height
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                    width: root.scrollBarWidth
                }

                Column {
                    id: graphColumn
                    width: parent.width - root.scrollBarWidth
                    spacing: 8

                    Text {
                        width: parent.width
                        height: 28
                        text: monthBox.currentText === "모든 날"
                              ? (dayBox.currentText === "모든 날" ? "월별 매출" : dayBox.currentText + "일 월별 매출")
                              : (dayBox.currentText === "모든 날" ? "일자별 매출" : "음료별 매출")
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                        color: "#1f344f"
                    }

                    Repeater {
                        model: root.graphData

                        Item {
                            width: graphColumn.width
                            height: 36

                            Text {
                                id: graphLabel
                                width: 78
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.label
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 12
                                color: "#222222"
                                elide: Text.ElideRight
                            }

                            Rectangle {
                                id: graphTrack
                                anchors.left: graphLabel.right
                                anchors.leftMargin: 10
                                anchors.right: graphValue.left
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                height: 18
                                radius: 3
                                color: "#edf1f6"
                                border.color: "#c5cfdb"
                                border.width: 1

                                Rectangle {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: root.graphMaxValue > 0
                                           ? Math.max(4, parent.width * modelData.value / root.graphMaxValue)
                                           : 0
                                    radius: 3
                                    color: "#5f8ec8"
                                }
                            }

                            Text {
                                id: graphValue
                                width: 86
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                text: root.formatNumber(modelData.value) + "원"
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 12
                                color: "#222222"
                                elide: Text.ElideRight
                            }
                        }
                    }

                    Text {
                        width: parent.width
                        height: root.graphData.length === 0 ? 80 : 0
                        visible: root.graphData.length === 0
                        text: "표시할 매출 데이터가 없습니다."
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 13
                        color: "#777777"
                    }
                }
            }
        }
    }
}
