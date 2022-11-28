include('common.py')
printGeometryGraph()
def gmlNode(i, edge="-"):
	return 'node [ id %d label "Z" ] edge [ source 0 target %d label "%s" ]' % (i, i, edge)

data = {}
data['any'] = 'node [ id 0 label "Q" stereo "any[]" ]'
data['any'] = 'node [ id 0 label "H" stereo "any[1]" ]' + gmlNode(1)
data['any'] = 'node [ id 0 label "Q" stereo "any[1, 2]" ]' + gmlNode(1) + gmlNode(2)
data['linear'] = 'node [ id 0 label "C" stereo "linear[1, 2]" ]' + gmlNode(1, "#") + gmlNode(2)
data['linear'] = 'node [ id 0 label "C"  stereo "linear[1, 2]" ]' + gmlNode(1, "=") + gmlNode(2, "=")
data['any'] = 'node [ id 0 label "Q" stereo "any[1, 2, 3]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3)
data['trigonalPlanar'] = 'node [ id 0 label "C" stereo "trigonalPlanar[1, 2, 3]" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3)
data['any'] = 'node [ id 0 label "Q" stereo "any[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4)
data['tetrahedral'] = 'node [ id 0 label "C" stereo "tetrahedral[1, 2, 3, 4]" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4)

post.summaryChapter("Graph")
for n, d in data.items():
	post.summarySection(n)
	gGML(d)
for side in ["context", "left", "right"]:
	post.summaryChapter("Rule " + side)
	for n, d in data.items():
		post.summarySection(n + " " + side)
		rGML(d, side)
