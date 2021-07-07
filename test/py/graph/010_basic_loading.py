include("../xxx_helpers.py")

dataGML = 'graph [ node [ id 0 label "C" ] ]'
fGML = 'myGraph.gml'
with open(fGML, "w") as f:
	f.write(dataGML)
fGML = CWDPath(fGML)
dataDFS = "C"
dataSMILES = dataDFS


def check(f, arg):
	def makeList(a):
		if isinstance(a, list):
			return a
		else:
			return [a]

	inputGraphs[:] = []
	assert inputGraphs == []
	res = makeList(f(arg))
	assert inputGraphs == res
	res2 = makeList(f(arg))
	assert inputGraphs == res + res2
	f(arg, add=False)
	assert inputGraphs == res + res2

check(Graph.fromGMLString, dataGML)
assert Graph.fromGMLString == graphGMLString

check(Graph.fromGMLStringMulti, dataGML)

check(Graph.fromGMLFile, fGML)
assert Graph.fromGMLFile == graphGML

check(Graph.fromGMLFileMulti, fGML)

check(Graph.fromDFS, dataDFS)
assert Graph.fromDFS == graphDFS

check(Graph.fromSMILES, dataSMILES)
assert Graph.fromSMILES == smiles

check(Graph.fromSMILESMulti, dataSMILES)


fail(lambda: Graph.fromGMLFile("doesNotExist.gml"),
	"Could not open graph GML file ", err=InputError, isSubstring=True)
