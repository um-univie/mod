include('common.py')
printGeometryGraph()
def gmlNode(i, edge="-"):
	return 'node [ id %d label "Z" ] edge [ source 0 target %d label "%s" ]' % (i, i, edge)

data = []
data.append(('any', 'node [ id 0 label "Q" stereo "[]" ]'))
data.append(('any', 'node [ id 0 label "H+" stereo "[]" ]'))
data.append(('any', 'node [ id 0 label "H" stereo "[1]" ]' + gmlNode(1)))
data.append(('any', 'node [ id 0 label "Q" stereo "[1, 2]" ]' + gmlNode(1) + gmlNode(2)))
data.append(('linear', 'node [ id 0 label "C" stereo "[1, 2]" ]' + gmlNode(1, "#") + gmlNode(2)))
data.append(('linear', 'node [ id 0 label "C"  stereo "[1, 2]" ]' + gmlNode(1, "=") + gmlNode(2, "=")))
data.append(('any', 'node [ id 0 label "Q" stereo "[1, 2, 3]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3)))
data.append(('trigonalPlanar', 'node [ id 0 label "C" stereo "[1, 2, 3]" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3)))
data.append(('trigonalPlanar', 'node [ id 0 label "C" stereo "[1, 2, 3]" ]' + gmlNode(1, ":") + gmlNode(2, ":") + gmlNode(3, ":")))
data.append(('trigonalPlanar', 'node [ id 0 label "N" stereo "[1, 2, 3]" ]' + gmlNode(1, ":") + gmlNode(2, ":") + gmlNode(3)))
data.append(('trigonalPlanar', 'node [ id 0 label "N" stereo "[1, 2, e]" ]' + gmlNode(1, ":") + gmlNode(2, ":")))
data.append(('any', 'node [ id 0 label "Q" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4)))
data.append(('tetrahedral', 'node [ id 0 label "C" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4)))
data.append(('tetrahedral', 'node [ id 0 label "N" stereo "[1, 2, 3, e]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3)))
data.append(('tetrahedral', 'node [ id 0 label "N+" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4)))
data.append(('tetrahedral', 'node [ id 0 label "P" stereo "[1, 2, 3, 4]" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3) + gmlNode(4)))
data.append(('tetrahedral', 'node [ id 0 label "P" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2, "=") + gmlNode(3) + gmlNode(4))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "P" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3, "=") + gmlNode(4))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "P" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4, "="))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1, "=") + gmlNode(2, "=") + gmlNode(3) + gmlNode(4)))
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3, "=") + gmlNode(4))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3) + gmlNode(4, "="))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2, "=") + gmlNode(3, "=") + gmlNode(4))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2, "=") + gmlNode(3) + gmlNode(4, "="))) # for testing initialisation
data.append(('tetrahedral', 'node [ id 0 label "S" stereo "[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3, "=") + gmlNode(4, "="))) # for testing initialisation

post.summaryChapter("Graph")
for n, d in data:
	post.summarySection(n)
	gGML(d)
for side in ["context", "left", "right"]:
	post.summaryChapter("Rule " + side)
	for n, d in data:
		post.summarySection(n + " " + side)
		rGML(d, side)
