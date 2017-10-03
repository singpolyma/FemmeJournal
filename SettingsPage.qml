import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Page {
	title: qsTr("Settings")

	Flickable {
		anchors.fill: parent
		contentHeight: contentItem.childrenRect.height

		GridLayout {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: 8
			rowSpacing: 1
			columns: 2

			Rectangle { Layout.columnSpan: 2; height: 8 }

			Label { text: qsTr("Weight Units") }

			ComboBox {
				Layout.alignment: Qt.AlignRight

				textRole: "label"
				model: ListModel {
					ListElement { label: qsTr("Kilograms"); value: "kg" }
					ListElement { label: qsTr("Pounds"); value: "lb" }
				}

				currentIndex: config.weightUnit == "lb" ? 1 : 0

				onActivated: {
					config.weightUnit = model.get(index).value
				}
			}

			Label { text: qsTr("Weight Units") }

			ComboBox {
				Layout.alignment: Qt.AlignRight

				textRole: "label"
				model: ListModel {
					ListElement { label: qsTr("Celcius"); value: "c" }
					ListElement { label: qsTr("Fahrenheit"); value: "f" }
				}

				currentIndex: config.temperatureUnit == "f" ? 1 : 0

				onActivated: {
					config.temperatureUnit = model.get(index).value
				}
			}

			Label { text: qsTr("Data File") }

			TextField {
				Layout.fillWidth: true
				horizontalAlignment: TextInput.AlignRight
				selectByMouse: true
				text: config.dataFilePath
				onEditingFinished: {
					config.dataFilePath = text
				}
			}

			Button {
				Layout.fillWidth: true
				Layout.columnSpan: 2

				text: qsTr("About / Credits")
				onClicked: {
					stack.push(creditsPage)
				}
			}
		}
	}
}
