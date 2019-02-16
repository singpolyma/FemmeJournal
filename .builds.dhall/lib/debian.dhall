\(releaseName: Text) -> \(extraDepends: List Text) -> {
	image = "debian/${releaseName}",
	packages = [
		"curl",
		"debhelper",
		"devscripts",
		"qtbase5-dev",
		"qtdeclarative5-dev",
		"libqt5svg5-dev",
		"qtdeclarative5-dev-tools"
	] # extraDepends,
	sources = ["https://git.sr.ht/~singpolyma/FemmeJournal"],
	tasks = [
		{ build =
			''
			cd FemmeJournal
			debuild -uc -us
			curl --upload-file ../femmejournal_*_*.deb https://transfer.sh/$(basename ../femmejournal_*_*.deb)
			''
		}
	]
}
