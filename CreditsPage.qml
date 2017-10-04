import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Page {
	title: qsTr("Credits")

	Flickable {
		anchors.fill: parent
		contentHeight: contentItem.childrenRect.height

		Text {
			anchors.left: parent.left
			anchors.right: parent.right
			padding: 10
			wrapMode: Text.WordWrap
			textFormat: Text.RichText
			text:
				"This application is free software, licensed under the GNU Affero General Public License, Version 3, or any later version." +
				"<ul>" +
				"<li><a href='https://ipfs.io/ipns/QmRdPza7cd6cBHHpRNusBz6w8zwXcxeD5NEgRyETRCfQyy'>Source code</a></li>" +
				"<li><a href='https://github.com/singpolyma/FemmeJournal/issues'>Issue tracker</a> / <a href='mailto:femmejournal@singpolyma.net'>Email issues</a></li>" +
				"<li>Donate: <a href='https://paypal.me/singpolyma'>PayPal</a> or <a href='bitcoin:1GHKCmFbgB9KXh7CHXqCVQY94E6EY8PxRC'>Bitcoin</a></li>" +
				"</ul>" +
				"<h2>Assets</h2>" +
				"<ul>" +
				"<li><a href='https://material.io/icons/'>arrowback.svg</a> Apache 2.0</li>" +
				"<li><a href='https://material.io/icons/'>settings.svg</a> Apache 2.0</li>" +
				"<li><a href='https://thenounproject.com/term/edit/1167409'>edit.svg</a> CC-BY 3.0 by Astonish</li>" +
				"<li><a href='https://thenounproject.com/term/calendar/5906'>calendar.svg</a> CC-BY 3.0 by Adi Dizdarevic</li>" +
				"<li><a href='https://thenounproject.com/term/heart/1009069'>heart.svg</a> CC-BY 3.0 by Aleksandr Vector</li>" +
				"<li><a href='https://thenounproject.com/term/flower/36206'>flower.svg</a> CC-BY 3.0 by Mister Pixel</li>" +
				"<li><a href='https://peppercarrot.com'>bg.png</a> CC-BY 4.0 by David Revoy</li>" +
				"</ul>"
			onLinkActivated: {
				Qt.openUrlExternally(link)
			}
		}
	}
}