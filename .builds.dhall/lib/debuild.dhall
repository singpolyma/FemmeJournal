let sedDepend = \(depend: Text) -> \(acc: Text) ->
	acc ++ "sed -i -e's/^Depends: /Depends: ${depend}, /' debian/control\n"
in
\(imageName: Text) -> \(extraBuildDepends: List Text) -> \(extraDepends: List Text) -> {
	image = imageName,
	packages = [
		"curl",
		"debhelper",
		"devscripts",
		"qtbase5-dev",
		"qtdeclarative5-dev",
		"libqt5svg5-dev",
		"qtdeclarative5-dev-tools"
	] # extraBuildDepends,
	sources = ["https://git.sr.ht/~singpolyma/FemmeJournal"],
	tasks = [
		{ build =
			''
			cd FemmeJournal
			${List/fold Text extraDepends Text sedDepend ""}
			debuild -uc -us
			curl -sT ../femmejournal_*_*.deb https://transfer.sh/$(basename ../femmejournal_*_*.deb)
			''
		}
	]
}
