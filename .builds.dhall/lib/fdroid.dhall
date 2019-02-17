{
	image = "debian/stable",
	packages = [
		"android-sdk",
		"android-sdk-platform-23",
		"curl",
		"google-android-ndk-installer",
		"inkscape"
	],
	repositories = {
		contrib = "http://ftp.ca.debian.org/debian/ stable contrib",
		backports = "http://ftp.ca.debian.org/debian/ stretch-backports main"
	},
	sources = ["https://git.sr.ht/~singpolyma/FemmeJournal"],
	tasks = [
		{ build =
			''
			sudo apt install -t stretch-backports fdroidserver
			wget https://github.com/dhall-lang/dhall-haskell/releases/download/1.20.1/dhall-json-1.20.1-x86_64-linux.tar.bz2
			tar -xvf dhall-json-1.20.1-x86_64-linux.tar.bz2
			export PATH="$(pwd)/bin:$PATH"
			cd FemmeJournal
			fdroid/gen dev
			export ANDROID_HOME=/usr/lib/android-sdk
			export ANDROID_NDK=/usr/lib/android-ndk
			fdroid build
			curl -sT unsigned/net.singpolyma.FemmeJournal_*.apk https://transfer.sh/$(basename unsigned/net.singpolyma.FemmeJournal_*.apk)
			DEBUG_BUILD="build/net.singpolyma.FemmeJournal/build/build/outputs/apk/build-debug.apk"
			[ -r "$DEBUG_BUILD" ] && curl -sT "$DEBUG_BUILD" https://transfer.sh/$(basename unsigned/net.singpolyma.FemmeJournal_*.apk)
			exit 0
			''
		}
	]
}
