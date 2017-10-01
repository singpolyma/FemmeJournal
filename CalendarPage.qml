import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0
import net.singpolyma.thefertilecycle 1.0

Page {
	title: qsTr("Calendar")
	property var journal: journal

	function intimateSummary(entry) {
		if(!entry.intimate) return null;
		var extras = [];
		if(entry.intimateProtection === true) extras.push(qsTr("Protected"))
		if(entry.intimateProtection === false) extras.push(qsTr("Unprotected"))
		if(entry.orgasm == JournalEntry.HadOrgasm) extras.push(qsTr("Orgasm"))
		if(entry.orgasm == JournalEntry.NoOrgasm) extras.push(qsTr("No Orgasm"))

		return qsTr("Intimate") + (extras.length < 1 ? "" : " (" + extras.join(", ") + ")");
	}

	function symptomsSummary(symptoms) {
		var summary = [];

		for(var i = 0; i < symptoms.rowCount(); i++) {
			var data = symptoms.data(symptoms.index(i, 0), SymptomsModel.SeverityRole);
			if(data !== null && data !== undefined) {
				summary.push(symptoms.data(symptoms.index(i, 0), SymptomsModel.SymptomRole));
			}
		}

		return summary.length > 0 ? qsTr("Symptoms: ") + summary.join(", ") : null;
	}

	function formatNumber(n, decimals) {
		var pointPattern = Qt.locale().decimalPoint.replace(/[\-\[\]\/\{\}\(\)\*\+\?\.\\\^\$\|]/g, "\\$&");
		return n.toLocaleString(Qt.locale(), "f", decimals).replace(new RegExp(pointPattern + "?0+$"), "");
	}

	function summaryText(entry) {
		if(!entry) return "";
		return [
			intimateSummary(entry),
			entry.opk == JournalEntry.OPKPositive ? qsTr("OPK: Positive") : null,
			entry.opk == JournalEntry.OPKNegative ? qsTr("OPK: Negative") : null,
			entry.temperature > 0 ? qsTr("Temperature: ") + formatNumber(entry.temperature, 2) + qsTr("°C") : null,
			entry.weight > 0 ? qsTr("Weight: ") + formatNumber(entry.weight, 2) + qsTr("kg") : null,
			symptomsSummary(entry.symptoms),
			"\n" + entry.note
		].filter(function(x) { return x; }).join("\n").trim();
	}

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
				text: summaryText(calendarModel.selectedJournal)
			}
		}

		EditJournalEntry {
			id: journal
			title: qsTr("Journal")
			z: -1
			Layout.fillHeight: true
			Layout.minimumWidth: parent.width > parent.height ? 0.3 * parent.width : 0
			visible: parent.width >= 1024
		}
	}
}
