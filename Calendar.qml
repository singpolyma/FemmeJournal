import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.calendar 1.0
import QtQuick.Controls.Material 2.0

Rectangle {
	color: Material.background

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		RowLayout {
			Layout.fillWidth: true

			ToolButton {
				text: qsTr("❮")
				onClicked: {
					if(cal.month === 0) {
						cal.month = 11
						calendarModel.month = 11
						cal.year--
						calendarModel.year--
					} else {
						cal.month--
						calendarModel.month--
					}
				}
			}

			Label {
				text: cal.title
				elide: Label.ElideRight
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
			}

			ToolButton {
				text: qsTr("❯")
				onClicked: {
					if(cal.month === 11) {
						cal.month = 0
						calendarModel.month = 0
						cal.year++
						calendarModel.year++
					} else {
						cal.month++
						calendarModel.month++
					}
				}
			}
		}

		DayOfWeekRow {
			Layout.fillWidth: true

			delegate: Label {
				// Can we make this use longName or narrowName when there is / is not space?
				text: model.shortName
				font.bold: false
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
		}

		MonthGrid {
			id: cal
			spacing: 1
			topPadding: 1
			bottomPadding: 1
			leftPadding: 1
			rightPadding: 1
			Layout.fillWidth: true
			Layout.fillHeight: true
			source: calendarModel

			background: Rectangle {
				color: materialContext.Material.frameColor
			}

			delegate: Rectangle {
				color: model.month === cal.month && model.date.toLocaleDateString() === calendarModel.selectedDate.toLocaleDateString() ? Qt.lighter(Material.primary) : Material.background

				Item {
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					opacity: model.month === cal.month ? 1 : 0

					RowLayout {
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.leftMargin: parent.width * 0.08
						height: Math.ceil(parent.height * 0.17)

						Text {
							Layout.fillHeight: true
							text: model.cycleDay
							font.pixelSize: parent.height
							color: materialContext.Material.secondaryTextColor
							opacity: model.period ? 1 : 0.5
						}

						Rectangle {
							Layout.fillWidth: true
							Layout.alignment: Qt.AlignVCenter
							implicitHeight: Math.ceil(parent.height / 2)
							color: Qt.rgba(1, 0, 0.2, 0.5)
							opacity: model.period ? 1 : 0
						}
					}

					Text {
						anchors.centerIn: parent
						horizontalAlignment: Text.AlignHCenter
						verticalAlignment: Text.AlignVCenter
						text: model.day
						font.pixelSize: Math.min(parent.height, parent.width) * 0.35
						font.bold: true
						color: Material.foreground
					}
				}
			}

			onClicked: {
				calendarModel.selectedDate = date;
			}
		}

	}
}
