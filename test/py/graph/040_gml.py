include("../xxx_helpers.py")

fail(lambda: Graph.fromGMLString('nope'),
	"Parsing failed", err=InputError, isSubstring=True)

fail(lambda: Graph.fromGMLString('nope []'),
	"Expected key", err=InputError, isSubstring=True)

fail(lambda: Graph.fromGMLString(
	'graph [ node [ id 0 label "C" ] node [ id 0 label "O" ] ]'),
	"used multiple times.", err=InputError, isSubstring=True)

dataDis = 'graph [ node [ id 0 label "C" ] node [ id 1 label "O" ] ]'
fail(lambda: Graph.fromGMLString(dataDis),
	"not connected", err=InputError, isSubstring=True)
gs = Graph.fromGMLStringMulti(dataDis)
assert len(gs) == 2

fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" ] edge [ source 0 target 0 label "-" ]
	]"""), "Loop edge", err=InputError, isSubstring=True)
fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" ] edge [ source 0 target 1 label "-" ]
	]"""), "Target 1 does not exist", err=InputError, isSubstring=True)
fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" ] edge [ source 1 target 0 label "-" ]
	]"""), "Source 1 does not exist", err=InputError, isSubstring=True)

fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" ] node [ id 1 label "C" ]
	edge [ source 0 target 1 label "-" ] edge [ source 1 target 0 label "-" ]
	]"""), "Duplicate edge", err=InputError, isSubstring=True)

# Stereo
fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" stereo "tetrahedral[1, 2, 3, 4]" ]
	]"""),
	"Neighbour vertex 1 in stereo embedding for vertex 0 does not exist.",
	err=InputError, isSubstring=True)

fail(lambda: Graph.fromGMLString("""graph [
	node [ id 0 label "C" stereo "tetrahedral[1, 2, 3, 4]" ]
	node [ id 1 label "O" ]
	]"""), "Vertex 1 in stereo embedding for vertex 0 is not a neighbour.",
	err=InputError, isSubstring=True)

Graph.fromGMLStringMulti("""graph [
	node [ id 0 label "C" stereo "linear" ]
	node [ id 1 label "H" ] node [ id 2 label "H" ]
	edge [ source 0 target 1 label "=" ]
	edge [ source 0 target 2 label "-" ] # to get a warning

	node [ id 10 label "C" stereo "linear" ]
	node [ id 11 label "H" ] node [ id 12 label "H" ]
	edge [ source 10 target 11 label "=" ]
	edge [ source 10 target 12 label "-" ] # to get a warning
]""")
