let Ref = { ref: Text, version: Text, debug: Bool }
let Indexed = \(t: Type) -> { index: Natural, value: t }
let map = https://ipfs.io/ipfs/Qmet1UAmpcY8iCWKNJy2SAk79LS88X7A1LyyFuREbs2zko
let fromOptional = https://ipfs.io/ipfs/QmTiAXe62AQA9UG5mPToE5yM98zBbgkJovKt7BVpXAqxzT
in
\(tags: List Ref) ->
	let indexedRefs = List/reverse (Indexed Ref) (List/indexed Ref tags)
	let latestRef = fromOptional (Indexed Ref) { index = 9999, value = { ref = "", version = "", debug = True } } (List/head (Indexed Ref) indexedRefs)
	in
	{
		Categories = ["Medical"],
		License = "AGPL-3.0-or-later",
		AuthorName = "singpolyma",
		AuthorEmail = "FemmeJournal@singpolyma.net",
		WebSite = "https://FemmeJournal.singpolyma.net",
		Donate = "https://paypal.me/singpolyma",
		LiberapayID = 1216655,
		Bitcoin = "1GHKCmFbgB9KXh7CHXqCVQY94E6EY8PxRC",

		SourceCode = "https://FemmeJournal.singpolyma.net",
		IssueTracker = "https://lists.sr.ht/~singpolyma/dev",

		AutoName = "FemmeJournal",
		Summary = "Cross-platform, convergence-ready, track menstruation and fertility",

		RepoType = "git",
		Repo = ".",

		Builds = map (Indexed Ref) ./buildType.dhall (\(ref: (Indexed Ref)) ->
			./build.dhall ref.value.ref ref.value.version (ref.index + 1) [5, 10, 1] "armv7" ref.value.debug
		) indexedRefs,

		AutoUpdateMode = "Version %v",
		UpdateCheckMode = "Tags",
		CurrentVersion = latestRef.value.version,
		CurrentVersionCode = latestRef.index + 1
	}
