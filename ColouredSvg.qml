import QtQuick 2.7
import QtGraphicalEffects 1.0

Image {
	id: svg
	property color color: "black"

	ColorOverlay {
		anchors.fill: parent
		source: parent
		color: svg.color
		opacity: parent.opacity
	}
}

