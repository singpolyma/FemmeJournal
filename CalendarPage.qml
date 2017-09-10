import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

Page {
	title: qsTr("Calendar")
	property var journal: journal

	GridLayout {
		columnSpacing: 0
		anchors.fill: parent
		flow: width > height ? GridLayout.LeftToRight : GridLayout.TopToBottom

		Calendar {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}

		Rectangle {
			color: materialContext.Material.dialogColor
			Layout.margins: parent.width * 0.02
			Layout.fillHeight: parent.width > parent.height
			Layout.fillWidth: parent.width <= parent.height
			Layout.preferredHeight: parent.width <= parent.height ? 0.3 * parent.height : 0
			Layout.preferredWidth: parent.width > parent.height ? 0.3 * parent.width : 0
			visible: parent.width < 1024

			Text {
				anchors.fill: parent
				elide: Text.ElideRight
				wrapMode: Text.Wrap
				text: [
					(calendarModel.selectedJournal.intimate ? "Intimate" : null),
					"\n" + calendarModel.selectedJournal.note
				].filter(function(x) { return x; }).join("\n")
			}
		}

		JournalEntry {
			id: journal
			title: qsTr("Journal")
			z: -1
			Layout.fillHeight: true
			Layout.minimumWidth: parent.width > parent.height ? 0.3 * parent.width : 0
			visible: parent.width >= 1024
		}
	}
}
