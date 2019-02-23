let concatMapSep = https://ipfs.io/ipfs/QmRVspXtiou4GV4aAEg7uRh3rYjTTMjuAkiJMRPAS961Fb
let qt = { version = [5, 10, 1], arch = "armv7" }
let androiddeployqt = \(debug: Bool) ->
	"$Qt5_android/bin/androiddeployqt --gradle ${if debug then "" else "--release"} --output build --input android-libFemmeJournal.so-deployment-settings.json"
in
\(commit: Text) -> \(versionName: Text) -> \(versionCode: Natural) -> \(qtVersion: List Natural) -> \(qtArch: Text) -> \(debug: Bool) -> {
	versionName = versionName,
	versionCode = versionCode,
	commit = commit,
	subdir = "android/",
	output = "../build/build/outputs/apk/build-release-unsigned.apk",
	sudo = "apt-get install -y librsvg2-bin",
	prebuild = ./prebuild.dhall (concatMapSep "" Natural Natural/show qtVersion) qtArch,
	build =
		''
		set -e
		export PATH="/tmp/QT/bin:$PATH"
		export Qt5_android=/tmp/QT/${concatMapSep "." Natural Natural/show qtVersion}/android_${qtArch}
		export ANDROID_NDK_ROOT=$$NDK$$
		export ANDROID_SDK_ROOT=$$SDK$$
		cd ..
		sed -i -e's/android:versionName="[^"]*"/android:versionName="${versionName}"/' android/AndroidManifest.xml
		sed -i -e's/android:versionCode="[^"]*"/android:versionCode="${Natural/show versionCode}"/' android/AndroidManifest.xml
		$Qt5_android/bin/qmake
		sed -i -e's/"debian"/"24.0.0"/' android-libFemmeJournal.so-deployment-settings.json
		make
		mkdir build
		make install INSTALL_ROOT=build
		${androiddeployqt False}
		${if debug then androiddeployqt True else ""}
		''
}: ./buildType.dhall
