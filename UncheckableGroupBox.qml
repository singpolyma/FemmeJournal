import QtQuick 2.7
import QtQuick.Controls 2.0

CheckBox {
	property bool previoulyChecked: false

	onPressed: {
		previoulyChecked = checked;
	}

	onReleased: {
		if(previoulyChecked) {
			checked = false
		}
	}
}
