include('common.py')
printGeometryGraph()
def gmlNode(i, edge="-"):
	return 'node [ id %d label "Z" ] edge [ source 0 target %d label "%s" ]' % (i, i, edge)

postSection('any')
gGML('node [ id 0 label "Q" stereo "[]!" ]')
gGML('node [ id 0 label "H+" stereo "[]!" ]')
gGML('node [ id 0 label "H" stereo "[1]!" ]' + gmlNode(1))
gGML('node [ id 0 label "Q" stereo "[1, 2]!" ]' + gmlNode(1) + gmlNode(2))

postSection('linear')
gGML('node [ id 0 label "C" stereo "[1, 2]!" ]' + gmlNode(1, "#") + gmlNode(2))
gGML('node [ id 0 label "C" stereo "[1, 2]!" ]' + gmlNode(1, "=") + gmlNode(2, "="))

postSection('any')
gGML('node [ id 0 label "Q" stereo "[1, 2, 3]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3))

postSection('trigonalPlanar')
gGML('node [ id 0 label "C" stereo "[1, 2, 3]!" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3))
gGML('node [ id 0 label "C" stereo "[1, 2, 3]!" ]' + gmlNode(1, ":") + gmlNode(2, ":") + gmlNode(3, ":"))
gGML('node [ id 0 label "N" stereo "[1, 2, 3]!" ]' + gmlNode(1, ":") + gmlNode(2, ":") + gmlNode(3))
gGML('node [ id 0 label "N" stereo "[1, 2, e]!" ]' + gmlNode(1, ":") + gmlNode(2, ":"))

postSection('any')
gGML('node [ id 0 label "Q" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4))

postSection('tetrahedral')
gGML('node [ id 0 label "C" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4))
gGML('node [ id 0 label "N" stereo "[1, 2, 3, e]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3))
gGML('node [ id 0 label "N+" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4))
gGML('node [ id 0 label "P" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3) + gmlNode(4))
gGML('node [ id 0 label "P" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2, "=") + gmlNode(3) + gmlNode(4)) # for testing initialisation
gGML('node [ id 0 label "P" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3, "=") + gmlNode(4)) # for testing initialisation
gGML('node [ id 0 label "P" stereo "[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4, "=")) # for testing initialisation
