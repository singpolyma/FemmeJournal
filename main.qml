import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

ApplicationWindow {
	visible: true
	width: 384
	height: 596
	title: qsTr("FemmeJournal")

	onClosing: {
		if(Qt.platform.os == "android" && stack.depth > 1) {
			close.accepted = false;
			stack.pop();
		}
	}

	header: ToolBar {
		id: materialContext

		RowLayout {
			anchors.fill: parent
			ToolButton {
				ColouredSvg {
					anchors.left: parent.left
					anchors.verticalCenter: parent.contentItem.verticalCenter
					source: "qrc:/arrowback.svg"
					sourceSize: Qt.size(parent.height/2, parent.height/2)
					color: materialContext.Material.primaryTextColor
				}

				text: stack.currentItem.title
				contentItem.anchors.right: right
				implicitWidth: contentItem.implicitWidth + (height/2) + 15
				onClicked: stack.pop()
				visible: stack.depth > 1
				font.capitalization: Font.MixedCase
			}
			Label {
				text: "      " + stack.currentItem.title
				visible: stack.depth < 2
			}
			Rectangle { Layout.fillWidth: true }
			ToolButton {
				onClicked: { calendarPage.journal.visible = true; stack.push(calendarPage.journal) }
				visible: parent.width < 1024 && stack.currentItem == calendarPage

				ColouredSvg {
					anchors.centerIn: parent
					source: "qrc:/edit.svg"
					sourceSize: Qt.size(parent.width/2, parent.height/2)
					color: materialContext.Material.primaryTextColor
				}
			}
			ToolButton {
				onClicked: { stack.push(calendarPage) }
				visible: stack.currentItem == summaryPage

				ColouredSvg {
					anchors.centerIn: parent
					source: "qrc:/calendar.svg"
					sourceSize: Qt.size(parent.width/2, parent.height/2)
					color: materialContext.Material.primaryTextColor
				}
			}

		}
	}

	StackView {
		id: stack
		anchors.fill: parent
		initialItem: Page {
			id: summaryPage
			title: qsTr("Summary")

			background: Image {
				source: "qrc:/bg.png"
				fillMode: Image.PreserveAspectCrop
			}

			ColumnLayout {
				height: parent.height > parent.width ? parent.height * 0.60 : parent.height
				width: parent.width

				Rectangle {
					Layout.topMargin: parent.parent.height * 0.05
					Layout.alignment: Qt.AlignHCenter
					radius: 10
					color: "#7F" + (""+materialContext.Material.background).substr(1,7)
					implicitWidth: projectedStart.paintedWidth * 1.10
					implicitHeight: projectedStart.paintedHeight * 1.10

					Text {
						anchors.centerIn: parent
						id: projectedStart
						text: qsTr("Projected start date: ") + calendarModel.nextCycle.toLocaleDateString(Locale.ShortFormat)
						font.pixelSize: parent.parent.width / 25
					}
				}

				Rectangle {
					Layout.alignment: Qt.AlignHCenter
					radius: 10
					color: "#7F" + (""+materialContext.Material.background).substr(1,7)
					implicitWidth: daysLeft.paintedWidth * 1.10
					implicitHeight: daysLeft.paintedHeight * 1.10

					Text {
						anchors.centerIn: parent
						id: daysLeft
						text: {
							var daysLeft = parseInt((calendarModel.nextCycle.getTime() - new Date().getTime()) / 86400000, 10);
							daysLeft > 0 ? daysLeft + qsTr(" Days Left") : (-1*daysLeft) + qsTr(" Days Late")
						}
						font.pixelSize: parent.parent.width / 10
					}
				}

				Button {
					Layout.alignment: Qt.AlignHCenter
					implicitWidth: col.implicitWidth * 1.10
					implicitHeight: col.implicitHeight * 1.50

					onClicked: {
						calendarModel.entryOf(new Date()).menstruationStarted = true;
					}

					Column {
						anchors.centerIn: parent
						id: col
						spacing: parent.parent.width / 50

						Text {
							id: colText
							text: qsTr("Period just started?")
							font.pixelSize: parent.parent.parent.width / 20
						}
						Text {
							width: colText.width
							horizontalAlignment: Text.AlignHCenter
							text: qsTr("tap here")
							font.pixelSize: parent.parent.parent.width / 40
						}
					}
				}
			}
		}

		CalendarPage {
			id: calendarPage
			visible: false
		}
	}
}
