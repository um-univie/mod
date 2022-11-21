include("../xxx_helpers.py")

dataGML = 'graph [ node [ id 0 label "C" ] ]'
fGML = 'myGraph.gml'
with open(fGML, "w") as f:
	f.write(dataGML)
fGML = CWDPath(fGML)


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


###############################################################################

check(Graph.fromGMLString, dataGML)
assert Graph.fromGMLString == graphGMLString

check(Graph.fromGMLFile, fGML)
assert Graph.fromGMLFile == graphGML
fail(lambda: Graph.fromGMLFile("doesNotExist.gml"),
	"Could not open GML file ", err=InputError, isSubstring=True)

check(Graph.fromGMLStringMulti, dataGML)
check(Graph.fromGMLFileMulti, fGML)
fail(lambda: Graph.fromGMLFileMulti("doesNotExist.gml"),
	"Could not open GML file ", err=InputError, isSubstring=True)

###############################################################################

dataDFS = "C"

check(Graph.fromDFS, dataDFS)
assert Graph.fromDFS == graphDFS

###############################################################################

dataSMILES = dataDFS

check(Graph.fromSMILES, dataSMILES)
assert Graph.fromSMILES == smiles

check(Graph.fromSMILESMulti, dataSMILES)

###############################################################################

dataMOL = """\n\n\n  1  0  0  0  0  0  0  0  0  0999 V3000
M  V30 BEGIN CTAB
M  V30 COUNTS 1 0 0 0 0
M  V30 BEGIN ATOM
M  V30 1 C 0 0 0 0
M  V30 END ATOM
M  V30 END CTAB
M  END"""
fMOL = 'myGraph.mol'
with open(fMOL, "w") as f:
	f.write(dataMOL)
fMOL = CWDPath(fMOL)

check(Graph.fromMOLString, dataMOL)
check(Graph.fromMOLFile, fMOL)
fail(lambda: Graph.fromMOLFile("doesNotExist.mol"),
	"Could not open MOL file ", err=InputError, isSubstring=True)

Graph.fromMOLStringMulti(dataMOL)
Graph.fromMOLFileMulti(fMOL)
fail(lambda: Graph.fromMOLFileMulti("doesNotExist.mol"),
	"Could not open MOL file ", err=InputError, isSubstring=True)

###############################################################################

dataSD = dataMOL + "\n$$$$"
fSD = 'myGraphs.sd'
with open(fSD, "w") as f:
	f.write(dataSD)
fSD = CWDPath(fSD)

check(Graph.fromSDString, dataSD)
check(Graph.fromSDFile, fSD)
fail(lambda: Graph.fromSDFile("doesNotExist.sd"),
	"Could not open SD file ", err=InputError, isSubstring=True)

Graph.fromSDStringMulti(dataSD)
Graph.fromSDFileMulti(fSD)
fail(lambda: Graph.fromSDFileMulti("doesNotExist.sd"),
	"Could not open SD file ", err=InputError, isSubstring=True)
