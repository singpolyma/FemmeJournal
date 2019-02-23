\(installer_code: Text) -> \(arch: Text) -> ''
# mostly taken from https://gitlab.com/ddorian/openrecipes/blob/master/android-setup-qt.sh
set -e

mkdir -p /tmp/QT

QT_INSTALLER="qt-unified-linux-x64-3.0.2-online.run"

cd "$(mktemp -d)"
wget -O "$QT_INSTALLER" "http://master.qt.io/archive/online_installers/3.0/$QT_INSTALLER"
chmod +x "$QT_INSTALLER"

QT_INSTALLER_SCRIPT="qt_installer_script.js"
cat <<- EOF > "$QT_INSTALLER_SCRIPT"
function Controller() {
  installer.autoRejectMessageBoxes();
  installer.installationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });

  var welcomePage = gui.pageWidgetByObjectName("WelcomePage");
  welcomePage.completeChanged.connect(function() {
    if (gui.currentPageWidget().objectName == welcomePage.objectName)
      gui.clickButton(buttons.NextButton);
  });
}

Controller.prototype.WelcomePageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.CredentialsPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
  gui.currentPageWidget().TargetDirectoryLineEdit.setText("/tmp/QT");
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
  var widget = gui.currentPageWidget();

  // You can get these component names by running the installer with the
  // --verbose flag. It will then print out a resource tree.

  widget.deselectAll();
  widget.selectComponent("qt.qt5.${installer_code}.android_${arch}");
  widget.selectComponent("qt.qt5.${installer_code}.qtcharts");

  gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
  gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
  gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
  var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm;
  if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox)
    checkBoxForm.launchQtCreatorCheckBox.checked = false;
  gui.clickButton(buttons.FinishButton);
}
EOF

QT_QPA_PLATFORM=minimal ./"$QT_INSTALLER" --script "$QT_INSTALLER_SCRIPT"

mkdir -p /tmp/QT/bin
echo "#!/bin/sh" >> /tmp/QT/bin/qmllint
chmod +x /tmp/QT/bin/qmllint
''
