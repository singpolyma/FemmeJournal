\(arch: Text) -> \(style: Text) -> {
	image = "debian/stretch",
	packages = [
		"curl",
		"imagemagick",
		"inkscape",
		"mxe-${arch}-w64-mingw32.${style}-qt5",
		"qtdeclarative5-dev-tools"
	],
	repositories = {
		mxe = "https://mirror.mxe.cc/repos/apt/ stretch main C6BF758A33A3A276"
	},
	sources = ["https://git.sr.ht/~singpolyma/FemmeJournal"],
	tasks = [
		{ build =
			''
			export PATH="/usr/lib/$(gcc -dumpmachine)/qt5/bin/:$PATH"
			export PATH="/usr/lib/mxe/usr/bin:$PATH"
			cd FemmeJournal
			${arch}-w64-mingw32.${style}-qmake-qt5
			make
			curl -sT FemmeJournal.exe https://transfer.sh/FemmeJournal.exe
			''
		}
	]
}
