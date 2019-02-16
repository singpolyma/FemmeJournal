import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import net.singpolyma.FemmeJournal 1.0

Page {
	title: qsTr("Stats")

	header: TabBar {
		id: tabs
		currentIndex: swipe.currentIndex

		Repeater {
			model: [qsTr("Cycles")].concat(QT_CHARTS_LIB ? [qsTr("Temperature Chart")] : [])

			TabButton { text: modelData }
		}
	}

	SwipeView {
		id: swipe
		anchors.fill: parent
		currentIndex: tabs.currentIndex

		Item {
			ListView {
				anchors.horizontalCenter: parent.horizontalCenter
				height: parent.height
				width: contentItem.childrenRect.width
				model: calendarModel.statsModel
				spacing: sizeHelper.font.pixelSize / 2

				Text {
					id: sizeHelper
					opacity: 0
					text: "Length: 000 days"
				}

				header: Column {
					id: column
					topPadding: sizeHelper.font.pixelSize
					bottomPadding: sizeHelper.font.pixelSize
					spacing: sizeHelper.font.pixelSize / 2

					RowLayout {
						width: periodLength.width

						Text {
							text: "Mean Cycle Length: "
							font.pixelSize: sizeHelper.font.pixelSize * 1.5
						}
						Text {
							text: "" + calendarModel.statsModel.meanCycleLength
							font.pixelSize: sizeHelper.font.pixelSize * 1.5
							horizontalAlignment: Text.AlignRight
							Layout.fillWidth: true
						}
					}

					RowLayout {
						id: periodLength

						Text {
							text: "Mean Period Length: "
							font.pixelSize: sizeHelper.font.pixelSize * 1.5
							width: periodLength.width
						}
						Text {
							text: "" + calendarModel.statsModel.meanMenstruationLength
							font.pixelSize: sizeHelper.font.pixelSize * 1.5
							horizontalAlignment: Text.AlignRight
							Layout.fillWidth: true
						}
					}
				}

				delegate: Row {
					spacing: sizeHelper.font.pixelSize * 2

					Text { text: cycle.date.toLocaleDateString(Qt.locale(), Locale.ShortFormat); font.weight: Font.Bold; width: parent.parent.width * 0.3 }
					RowLayout {
						width: sizeHelper.width

						Text { text: "Length: " }
						Text { text: cycle.length + " days"; horizontalAlignment: Text.AlignRight; Layout.fillWidth: true }
					}
				}
			}
		}

		Loader { source: QT_CHARTS_LIB ? "qrc:/TemperatureChart.qml" : "" }
	}
}
