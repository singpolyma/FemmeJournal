import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

Item {
	property color color: materialContext.Material.buttonColor
	property color backgroundColor: Material.background
	property color borderColor: Material.foreground
	property color currentColor: { var x = parent.checked ? backgroundColor : color; x.a = 1; x }
	property real radius: 5
	property string radiusOn: "none"

	Rectangle {
		anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
		width: parent.width / 2 + parent.radius
		radius: radiusOn === "left" ? parent.radius : 0
		border.width: 1
		border.color: borderColor
		color: currentColor
	}

	Rectangle {
		anchors { top: parent.top; bottom: parent.bottom; right: parent.right; rightMargin: radiusOn === "right" ? 0 : -1 }
		width: parent.width / 2 + parent.radius
		radius: radiusOn === "right" ? parent.radius : 0
		border.width: 1
		border.color: borderColor
		color: currentColor
	}

	Rectangle {
		anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
		height: parent.height - 2;
		width: parent.radius * 2 + 2
		color: currentColor
	}
}
