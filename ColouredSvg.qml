import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
	property color color: "black"
	property alias source: svg.source
	property alias sourceSize: svg.sourceSize
	property alias mirror: svg.mirror
	property alias fillMode: svg.fillMode
	implicitWidth: svg.implicitWidth
	implicitHeight: svg.implicitHeight

	Image {
		id: svg
		anchors.centerIn: parent
		opacity: parent.opacity
		layer.enabled: true
	}

	ColorOverlay {
		anchors.fill: parent
		source: svg
		color: parent.color
		opacity: svg.opacity
	}
}
