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
				Layout.minimumHeight: font.pixelSize * 3
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
			model: calendarModel.selectedJournal.symptoms

			header: Rectangle {
				property var label: mediumLabel
				anchors.left: parent.left
				anchors.right: parent.right
				height: mediumLabel.font.pixelSize * 3
				z: 5
				color: materialContext.Material.background

				RowLayout {
					anchors.fill: parent
					anchors.rightMargin: 5
					anchors.leftMargin: 5

					Rectangle {
						Layout.fillWidth: true
					}

					Rectangle {
						Layout.preferredWidth: label.implicitWidth
					}

					Label {
						Layout.preferredWidth: label.implicitWidth
						horizontalAlignment: Text.AlignHCenter
						text: qsTr("Light")
					}
					Label {
						horizontalAlignment: Text.AlignHCenter
						id: mediumLabel
						text: qsTr("Medium")
					}
					Label {
						Layout.preferredWidth: label.implicitWidth
						horizontalAlignment: Text.AlignHCenter
						text: qsTr("Heavy")
					}
				}
			}

			headerPositioning: ListView.OverlayHeader

			delegate: GridLayout {
				id: delegate
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.rightMargin: 5
				anchors.leftMargin: 5
				columns: 5
				rowSpacing: 0

				Text {
					Layout.fillWidth: true
					text: model.symptom
					elide: Text.ElideRight
					font.capitalization: Font.Capitalize
				}

				CheckBox {
					implicitWidth: delegate.ListView.view.headerItem.label.width
					checked: model.severity !== null && model.severity !== undefined
					onCheckedChanged: {
						var severity = model.severity ? model.severity : SymptomsModel.Unknown;
						calendarModel.selectedJournal.symptoms.setData(model.index, checked ? severity : undefined, SymptomsModel.SeverityRole)
					}
				}

				RadioButton {
					implicitWidth: delegate.ListView.view.headerItem.label.width
					checked: model.severity == SymptomsModel.Light
					onCheckedChanged: {
						if (checked) calendarModel.selectedJournal.symptoms.setData(model.index, SymptomsModel.Light, SymptomsModel.SeverityRole)
					}
				}

				RadioButton {
					implicitWidth: delegate.ListView.view.headerItem.label.width
					checked: model.severity == SymptomsModel.Medium
					onCheckedChanged: {
						if (checked) calendarModel.selectedJournal.symptoms.setData(model.index, SymptomsModel.Medium, SymptomsModel.SeverityRole)
					}
				}

				RadioButton {
					implicitWidth: delegate.ListView.view.headerItem.label.width
					checked: model.severity == SymptomsModel.Heavy
					onCheckedChanged: {
						if (checked) calendarModel.selectedJournal.symptoms.setData(model.index, SymptomsModel.Heavy, SymptomsModel.SeverityRole)
					}
				}

				Rectangle {
					Layout.fillWidth: true
					Layout.columnSpan: 5
					height: 1
					color: "#000000"
				}
			}
		}

		Item {
			GridLayout {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.margins: 8
				rowSpacing: 1
				columns: 2

				Rectangle { Layout.columnSpan: 2; height: 8 }

				Label { text: qsTr("Temperature") }

				TextField {
					Layout.alignment: Qt.AlignRight
					horizontalAlignment: TextInput.AlignRight
					selectByMouse: true
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					validator: DoubleValidator {
						bottom: 0
						decimals: 5
						notation: DoubleValidator.StandardNotation
					}
					text: calendarModel.selectedJournal.temperature === 0 ? "" : calendarModel.selectedJournal.temperature.toLocaleString(Qt.locale(), "f", 5).replace(/\.?0+$/, '')
					onEditingFinished: {
						calendarModel.selectedJournal.temperature = Number.fromLocaleString(Qt.locale(), text)
					}
				}

				Label { text: qsTr("Weight") }

				TextField {
					Layout.alignment: Qt.AlignRight
					horizontalAlignment: TextInput.AlignRight
					selectByMouse: true
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					validator: DoubleValidator {
						bottom: 0
						decimals: 5
						notation: DoubleValidator.StandardNotation
					}
					text: calendarModel.selectedJournal.weight === 0 ? "" : calendarModel.selectedJournal.weight.toLocaleString(Qt.locale(), "f", 5).replace(/\.?0+$/, '')
					onEditingFinished: {
						calendarModel.selectedJournal.weight = Number.fromLocaleString(Qt.locale(), text)
					}
				}

				Label { text: qsTr("Ovulation Prediction Kit") }

				ComboBox {
					id: opkSelect
					Layout.alignment: Qt.AlignRight

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
}
