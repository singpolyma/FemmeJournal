\(releaseName: Text) -> \(extraDepends: List Text) ->
	./debuild.dhall "ubuntu/${releaseName}" extraDepends
