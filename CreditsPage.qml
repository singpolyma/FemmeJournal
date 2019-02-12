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
				"Version: " + VERSION +
				"<p>This application is free software, licensed under the <a href='https://www.gnu.org/licenses/agpl-3.0.en.html'>GNU Affero General Public License, Version 3</a>, or any later version.</p>" +
				"<ul>" +
				"<li><a href='https://ipfs.io/ipns/12D3KooWQSeeZbkkZFsgAbDC1sF1dyVmHtsyngVjErv5Y1SV99KN' title='" + COMMIT + "'>Source code (git)</a> </li>" +
				"<li>Publicly email bugs, feedback, and patches to: <a href='mailto:FemmeJournal@singpolyma.net'>FemmeJournal@singpolyma.net</a> </li>" +
				"<li>Donate: <a href='https://liberapay.com/singpolyma'>Regular support</a> or <a href='https://paypal.me/singpolyma'>one-time PayPal</a> or <a href='bitcoin:1GHKCmFbgB9KXh7CHXqCVQY94E6EY8PxRC'>Bitcoin</a> </li>" +
				"</ul>" +
				"<h2>Assets</h2>" +
				"<ul>" +
				"<li><a href='https://material.io/icons/'>arrowback.svg</a> Apache 2.0</li>" +
				"<li><a href='https://material.io/icons/'>settings.svg</a> Apache 2.0</li>" +
				"<li><a href='https://material.io/icons/'>chart.svg</a> Apache 2.0</li>" +
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
