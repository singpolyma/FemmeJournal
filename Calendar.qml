import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.calendar 1.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0

Rectangle {
	color: Material.background

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		RowLayout {
			Layout.fillWidth: true

			ToolButton {
				ColouredSvg {
					anchors.centerIn: parent
					source: "qrc:/arrowback.svg"
					sourceSize: Qt.size(parent.height/2, parent.height/2)
					color: Material.foreground
				}

				onClicked: {
					if(cal.month === 0) {
						cal.month = 11
						calendarModel.month = 12
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
				ColouredSvg {
					anchors.centerIn: parent
					source: "qrc:/arrowback.svg"
					sourceSize: Qt.size(parent.height/2, parent.height/2)
					mirror: true
					color: Material.foreground
				}

				onClicked: {
					if(cal.month === 11) {
						cal.month = 0
						calendarModel.month = 1
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
				color: model.month === cal.month && model.date == calendarModel.selectedDate ? Qt.lighter(Material.primary) : Material.background

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
							opacity: model.cycleDay == 0 ? 0 : (model.menstruating ? 1 : 0.5)
						}

						Rectangle {
							Layout.fillWidth: true
							Layout.alignment: Qt.AlignVCenter
							implicitHeight: Math.ceil(parent.height / 2)
							color: Qt.rgba(1, 0, 0.2, 0.5)
							opacity: model.menstruating ? 1 : 0
						}

						Rectangle {
							Layout.margins: 2
							implicitHeight: parent.height
							implicitWidth: height
							radius: width*0.5
							color: "#84aa41"
							opacity: 0.75
							visible: model.fertility == "fertile"
						}

						ColouredSvg {
							Layout.margins: 2
							Layout.alignment: Qt.AlignRight
							source: "qrc:/flower.svg"
							Layout.preferredWidth: parent.height
							Layout.preferredHeight: parent.height
							sourceSize.width: parent.height
							sourceSize.height: parent.height
							fillMode: Image.PreserveAspectFit
							opacity: 0.5
							visible: model.fertility == "ovulated"
							color: Material.foreground
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

					RowLayout {
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.bottom: parent.bottom
						anchors.margins: 2
						height: Math.ceil(parent.height * 0.17)

						Item {
							implicitHeight: parent.height
							implicitWidth: height
							opacity: summaryText(model.journalEntry) == "" ? 0 : 0.5

							Rectangle {
								id: summaryCircle
								anchors.centerIn: parent
								implicitHeight: parent.parent.height
								implicitWidth: height
								radius: width*0.5
								color: "black"
								opacity: 0.5
							}

							ColorOverlay {
								anchors.fill: parent
								source: summaryCircle
								opacity: 0.5
								color: Material.foreground
							}
						}

						ColouredSvg {
							Layout.alignment: Qt.AlignRight
							source: "qrc:/heart.svg"
							Layout.preferredWidth: parent.height
							Layout.preferredHeight: parent.height
							sourceSize.width: parent.height
							sourceSize.height: parent.height
							fillMode: Image.PreserveAspectFit
							opacity: (model.journalEntry && model.journalEntry.intimate) ? 0.5 : 0
							color: Material.foreground
						}
					}
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						calendarModel.selectedDate = model.date;
					}
				}
			}

			onClicked: {
				focus = true;
				// Don't use the date argument here, it makes use of
				// undefined behaviour: https://bugreports.qt.io/browse/QTBUG-29328
			}
		}

	}
}
