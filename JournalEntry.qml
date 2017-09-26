import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.calendar 1.0
import QtQuick.Controls.Material 2.0
import net.singpolyma.thefertilecycle 1.0

Page {
	header: TabBar {
		id: tabs
		currentIndex: swipe.currentIndex

		TabButton { text: qsTr("Notes") }
		TabButton { text: qsTr("Symptoms") }
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
				leftPadding: 8
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

				ColumnLayout {
					Layout.fillWidth: true
					Layout.columnSpan: 2
					Layout.leftMargin: parent.parent.parent.width * 0.02
					Layout.rightMargin: 8 + parent.parent.parent.width * 0.02
					visible: calendarModel.selectedJournal.intimate

					Row {
						Layout.fillWidth: true

						Button {
							checkable: true
							implicitHeight: font.pixelSize * 2
							implicitWidth: parent.width / 2
							text: "Protected"
							font.capitalization: Font.MixedCase
							background: Segment {
								radiusOn: "left"
							}

							checked: calendarModel.selectedJournal.intimateProtection === true
							onClicked: {
								calendarModel.selectedJournal.intimateProtection =
									calendarModel.selectedJournal.intimateProtection === true ? undefined : true;
								checked = Qt.binding(function () { return calendarModel.selectedJournal.intimateProtection === true; });
							}
						}
						Button {
							checkable: true
							implicitHeight: font.pixelSize * 2
							implicitWidth: parent.width / 2
							text: "Unprotected"
							font.capitalization: Font.MixedCase
							background: Segment {
								radiusOn: "right"
							}

							checked: calendarModel.selectedJournal.intimateProtection === false
							onClicked: {
								calendarModel.selectedJournal.intimateProtection =
									calendarModel.selectedJournal.intimateProtection === false ? undefined : false;
								checked = Qt.binding(function () { return calendarModel.selectedJournal.intimateProtection === false; });
							}
						}
					}

					Row {
						Layout.fillWidth: true

						Button {
							checkable: true
							implicitHeight: font.pixelSize * 2
							implicitWidth: parent.width / 2
							text: "Orgasm"
							font.capitalization: Font.MixedCase
							background: Segment {
								radiusOn: "left"
							}

							checked: calendarModel.selectedJournal.orgasm === JournalEntry.HadOrgasm
							onClicked: {
								calendarModel.selectedJournal.orgasm =
									calendarModel.selectedJournal.orgasm === JournalEntry.HadOrgasm ? JournalEntry.OrgasmUnknown : JournalEntry.HadOrgasm;
								checked = Qt.binding(function () { return calendarModel.selectedJournal.orgasm === JournalEntry.HadOrgasm; });
							}
						}
						Button {
							checkable: true
							implicitHeight: font.pixelSize * 2
							implicitWidth: parent.width / 2
							text: "No Orgasm"
							font.capitalization: Font.MixedCase
							background: Segment {
								radiusOn: "right"
							}

							checked: calendarModel.selectedJournal.orgasm == JournalEntry.NoOrgasm
							onClicked: {
								calendarModel.selectedJournal.orgasm =
									calendarModel.selectedJournal.orgasm === JournalEntry.NoOrgasm ? JournalEntry.OrgasmUnknown : JournalEntry.NoOrgasm;
								checked = Qt.binding(function () { return calendarModel.selectedJournal.orgasm === JournalEntry.NoOrgasm; });
							}
						}
					}

					Rectangle { height: 5 }
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

		Item {
			ComboBox {
				id: opkSelect

				textRole: "label"
				model: ListModel {
					ListElement { label: qsTr("None"); value: JournalEntry.OPKNone }
					ListElement { label: qsTr("Negative"); value: JournalEntry.OPKNegative }
					ListElement { label: qsTr("Positive"); value: JournalEntry.OPKPositive }
				}

				Binding {
					target: opkSelect
					property: "currentIndex"
					value: {
						var idx = null;

						for(var i = 0; i < opkSelect.model.count; i++) {
							if(opkSelect.model.get(i).value == calendarModel.selectedJournal.opk) {
								idx = i;
								break;
							}
						}

						return idx;
					}
				}

				onActivated: {
					calendarModel.selectedJournal.opk = model.get(index).value
				}
			}
		}
	}
}
