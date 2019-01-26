import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

ToolBar {
	RowLayout {
		anchors.fill: parent
		ToolButton {
			ColouredSvg {
				anchors.left: parent.left
				anchors.verticalCenter: parent.contentItem.verticalCenter
				source: "qrc:/arrowback.svg"
				sourceSize: Qt.size(parent.height/2, parent.height/2)
				color: materialContext.Material.foreground
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
			onClicked: { stack.push(settingsPage) }
			visible: stack.currentItem != settingsPage && stack.currentItem != creditsPage

			ColouredSvg {
				anchors.centerIn: parent
				source: "qrc:/settings.svg"
				sourceSize: Qt.size(parent.width/2, parent.height/2)
				color: materialContext.Material.foreground
			}
		}
		ToolButton {
			onClicked: { calendarPage.journal.visible = true; stack.push(calendarPage.journal) }
			visible: parent.width < 1024 && stack.currentItem == calendarPage

			ColouredSvg {
				anchors.centerIn: parent
				source: "qrc:/edit.svg"
				sourceSize: Qt.size(parent.width/2, parent.height/2)
				color: materialContext.Material.foreground
			}
		}
		ToolButton {
			onClicked: { stack.push(statsPage) }
			visible: stack.currentItem == summaryPage

			ColouredSvg {
				anchors.centerIn: parent
				source: "qrc:/chart.svg"
				sourceSize: Qt.size(parent.width/2, parent.height/2)
				color: materialContext.Material.foreground
			}
		}
		ToolButton {
			onClicked: { stack.push(calendarPage) }
			visible: stack.currentItem == summaryPage

			ColouredSvg {
				anchors.centerIn: parent
				source: "qrc:/calendar.svg"
				sourceSize: Qt.size(parent.width/2, parent.height/2)
				color: materialContext.Material.foreground
			}
		}
	}
}
