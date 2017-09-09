import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.calendar 1.0
import QtQuick.Controls.Material 2.0

Page {
	header: TabBar {
		id: tabs
		currentIndex: swipe.currentIndex

		TabButton { text: qsTr("Notes") }
		TabButton { text: qsTr("Symptoms") }
		TabButton { text: qsTr("Moods") }
		TabButton { text: qsTr("Others") }
	}

	SwipeView {
		id: swipe
		anchors.fill: parent
		currentIndex: tabs.currentIndex

		ColumnLayout {
			spacing: 0

			TextArea {
				// TODO: Not currently scrollable
				Layout.fillHeight: true
				Layout.fillWidth: true
				Layout.bottomMargin: parent.parent.height * 0.1
				wrapMode: TextEdit.Wrap
				placeholderText: qsTr("Notes")
				selectByMouse: true
				text: calendarModel.selectedJournal.note
				onTextChanged: {
					calendarModel.selectedJournal.note = text
				}
			}

			Rectangle {
				Layout.fillWidth: true
				height: 1
				color: "#000000"
			}

			GridLayout {
				Layout.fillWidth: true
				Layout.leftMargin: 8
				rowSpacing: 1
				columns: 2

				Label {
					Layout.fillWidth: true
					elide: Text.ElideRight
					text: qsTr("Intimate Today")
				}
				CheckBox {
					checked: calendarModel.selectedJournal.intimate
					onCheckedChanged: {
						calendarModel.selectedJournal.intimate = checked
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.columnSpan: 2
					height: 1
					color: "#000000"
				}

				Label {
					Layout.fillWidth: true
					elide: Text.ElideRight
					text: qsTr("Period Started Today")
				}
				CheckBox {
					checked: calendarModel.selectedJournal.menstruationStarted
					onCheckedChanged: {
						calendarModel.selectedJournal.menstruationStarted = checked
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.columnSpan: 2
					height: 1
					color: "#000000"
				}

				Label {
					Layout.fillWidth: true
					elide: Text.ElideRight
					text: qsTr("Period Ended Today")
				}
				CheckBox {
					checked: calendarModel.selectedJournal.menstruationStopped
					onCheckedChanged: {
						calendarModel.selectedJournal.menstruationStopped = checked
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.columnSpan: 2
					height: 1
					color: "#000000"
				}

				Label {
					Layout.fillWidth: true
					elide: Text.ElideRight
					text: qsTr("Ovulated Today")
				}
				CheckBox {
					checked: calendarModel.selectedJournal.ovulated
					onCheckedChanged: {
						calendarModel.selectedJournal.ovulated = checked
					}
				}
			}
		}

		ListView {
			model: ListModel {
				ListElement { name: "Acne" }
			}

			delegate: GridLayout {
				anchors.left: parent.left
				anchors.right: parent.right
				columns: 2

				Text {
					Layout.fillWidth: true
					text: model.name
				}

				RowLayout {
					Layout.fillWidth: true
					spacing: 0

					TabButton {
						Layout.fillWidth: true
						text: "1"
						background: Segment {
							radiusOn: "left"
						}
					}
					TabButton {
						Layout.fillWidth: true
						text: "2"
						background: Segment {}
					}
					TabButton {
						Layout.fillWidth: true
						text: "3"
						background: Segment {
							radiusOn: "right"
						}
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.columnSpan: 2
					height: 1
					color: "#000000"
				}
			}
		}

		Item {}
		Item {}
	}
}
