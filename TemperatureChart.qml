import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtCharts 2.2

ChartView {
	anchors.fill: parent
	antialiasing: true
	legend.visible: false
	margins.top: backbutton.height

	LineSeries {
		axisX: DateTimeAxis {
			format: "MMM dd"
			min: temperatureChartModel.start
			max: temperatureChartModel.end
		}

		axisY: ValueAxis {
			min: temperatureChartModel.yMin
			max: temperatureChartModel.yMax
		}

		VXYModelMapper {
			model: temperatureChartModel
			xColumn: 0
			yColumn: 1
		}
	}

	ToolButton {
		id: backbutton
		anchors.top: parent.top
		anchors.topMargin: height * 0.1
		anchors.left: parent.left
		anchors.leftMargin: height * 0.1

		ColouredSvg {
			anchors.centerIn: parent
			source: "qrc:/arrowback.svg"
			sourceSize: Qt.size(parent.height/2, parent.height/2)
			color: Material.foreground
		}

		onClicked: {
			temperatureChartModel.end = new Date(temperatureChartModel.start.getTime() - 24*60*60*1000);
		}
	}

	ToolButton {
		anchors.top: parent.top
		anchors.topMargin: height * 0.1
		anchors.right: parent.right
		anchors.rightMargin: height * 0.1

		ColouredSvg {
			anchors.centerIn: parent
			source: "qrc:/arrowback.svg"
			sourceSize: Qt.size(parent.height/2, parent.height/2)
			mirror: true
			color: Material.foreground
		}

		onClicked: {
			temperatureChartModel.end = new Date(temperatureChartModel.end.getTime() + 28*24*60*60*1000);
		}
	}
}
