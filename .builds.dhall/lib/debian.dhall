\(releaseName: Text) -> \(extraDepends: List Text) ->
	./debuild.dhall "debian/${releaseName}" extraDepends
