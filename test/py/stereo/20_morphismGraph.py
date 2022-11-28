# TODO: check different number of lonePair and radial.
#       Too many morphisms are accepted now.
include("common.py")
printGeometryGraph()

def gmlNode(i, edge="-", stereo=""):
	return 'node [ id %d label "Z" ] edge [ source 0 target %d label "%s" %s ]' % (i, i, edge, stereo)
def gmlNodeIdx(i, edge="-", stereo=""):
	return 'node [ id %d label "Z%d" ] edge [ source 0 target %d label "%s" %s ]' % (i, i, i, edge, stereo)

def makeAny(n):
	gml = 'node [ id 0 label "Q" stereo "any" ]'
	for i in range(1, n + 1):
		gml += gmlNode(i, edge="*", stereo='stereo "*"' )
	return gGML(gml)

maxDeg = 4
def makeGraphDict():
	graphs = {}
	for i in range(maxDeg + 1):
		graphs["any%d" % i] = makeAny(i)

	graphs["linear"] = gGML('node [ id 0 label "Q" stereo "linear" ]' + gmlNode(1) + gmlNode(2))
	graphs["trigonalPlanar"] = gGML('node [ id 0 label "Q" stereo "trigonalPlanar" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3))
	#graphs["trigonalPlanarFixed"] = gGML('node [ id 0 label "Q" stereo "trigonalPlanar[1, 2, 3]!" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3))
	# TODO: enable trigonal planer with fixed configuration
	graphs["tetrahedral"] = gGML('node [ id 0 label "Q" stereo "tetrahedral" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4))
	graphs["tetrahedralFixed"] = gGML('node [ id 0 label "Q" stereo "tetrahedral[1, 2, 3, 4]!" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4))
	return graphs

print("Isomorphism")
graphs1 = makeGraphDict()
graphs2 = makeGraphDict()
for n1, g1 in sorted(graphs1.items()):
	for n2, g2 in sorted(graphs2.items()):
		def check(res):
			if not res:
				post.summaryChapter("Error: %s, %s" % (n1, n2))
				post.summarySection("Stereo, %s" % n1)
				printStereo(g1)
				post.summarySection("Stereo, %s" % n2)
				printStereo(g2)
				assert False
		if n1 == n2:
			res = g1.isomorphism(g2, labelSettings=isoLabelSettings) > 0
			check(res)
			res = g2.isomorphism(g1, labelSettings=isoLabelSettings) > 0
			check(res)
		else:
			res = g1.isomorphism(g2, labelSettings=isoLabelSettings) == 0
			check(res)
			res = g2.isomorphism(g1, labelSettings=isoLabelSettings) == 0
			check(res)

print("Specialisation")
ok = []
for i in range(maxDeg + 1):
	for j in range(i, maxDeg + 1):
		ok.append(("any%d" % i, "any%d" % j))
for l in ["linear"]:
	for i in range(3):
		ok.append(("any%d" % i, l))
for l in ["trigonalPlanar"]: # trigonalPlanarFixed
	for i in range(4):
		ok.append(("any%d" % i, l))
for l in ["tetrahedral", "tetrahedralFixed"]:
	for i in range(5):
		ok.append(("any%d" % i, l))
ok.append(("tetrahedral", "tetrahedralFixed"))
graphs1 = makeGraphDict()
graphs2 = makeGraphDict()
for n1, g1 in sorted(graphs1.items()):
	for n2, g2 in sorted(graphs2.items()):
		if n1 == n2:
			def check(res):
				if not res:
					post.summaryChapter("Error")
					printStereo(g1)
					printStereo(g2)
					assert False
			res = g1.isomorphism(g2, labelSettings=specLabelSettings) > 0
			check(res)
			res = g2.isomorphism(g1, labelSettings=specLabelSettings) > 0
			check(res)
		else:
			def check(res, n1, n2):
				isOk = (n1, n2) in ok
				if isOk != res:
					post.summaryChapter("Error: %d" % res)
					print(n1, "vs.", n2)
					print("isOk:", isOk)
					print("res:", res)
					g1.print()
					printStereo(g1)
					g2.print()
					printStereo(g2)
					assert False
			res = g1.monomorphism(g2, labelSettings=specLabelSettings) > 0
			check(res, n1, n2)
			res = g2.monomorphism(g1, labelSettings=specLabelSettings) > 0
			check(res, n2, n1)


print("Tetrahedral")
graphs1 = {}
graphs2 = {}
tf1 = graphs1["t_f"] = gGML('node [ id 0 label "Q" stereo "tetrahedral" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))
tf2 = graphs2["t_f"] = gGML('node [ id 0 label "Q" stereo "tetrahedral" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))
ta1 = graphs1["t_a"] = gGML('node [ id 0 label "Q" stereo "tetrahedral[1, 2, 3, 4]!" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))
ta2 = graphs2["t_a"] = gGML('node [ id 0 label "Q" stereo "tetrahedral[1, 2, 3, 4]!" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))
tb1 = graphs1["t_b"] = gGML('node [ id 0 label "Q" stereo "tetrahedral[1, 2, 4, 3]!" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))
tb2 = graphs2["t_b"] = gGML('node [ id 0 label "Q" stereo "tetrahedral[1, 2, 4, 3]!" ]' + gmlNodeIdx(1) + gmlNodeIdx(2) + gmlNodeIdx(3) + gmlNodeIdx(4))

print("\tIsomorphism")
post.summaryChapter("Tetrahedral Isomorphism")
assert tf1.isomorphism(tf2, labelSettings=isoLabelSettings) > 0
assert tf1.isomorphism(ta2, labelSettings=isoLabelSettings) == 0
assert tf1.isomorphism(tb2, labelSettings=isoLabelSettings) == 0
assert ta1.isomorphism(tf2, labelSettings=isoLabelSettings) == 0
assert ta1.isomorphism(ta2, labelSettings=isoLabelSettings) > 0
assert ta1.isomorphism(tb2, labelSettings=isoLabelSettings) == 0
assert tb1.isomorphism(tf2, labelSettings=isoLabelSettings) == 0
assert tb1.isomorphism(ta2, labelSettings=isoLabelSettings) == 0
assert tb1.isomorphism(tb2, labelSettings=isoLabelSettings) > 0

print("\tSpecialisation")
post.summaryChapter("Tetrahedral Specialisation")
assert tf1.isomorphism(tf2, labelSettings=specLabelSettings) > 0
assert tf1.isomorphism(ta2, labelSettings=specLabelSettings) > 0
assert tf1.isomorphism(tb2, labelSettings=specLabelSettings) > 0
assert ta1.isomorphism(tf2, labelSettings=specLabelSettings) == 0
assert ta1.isomorphism(ta2, labelSettings=specLabelSettings) > 0
assert ta1.isomorphism(tb2, labelSettings=specLabelSettings) == 0
assert tb1.isomorphism(tf2, labelSettings=specLabelSettings) == 0
assert tb1.isomorphism(ta2, labelSettings=specLabelSettings) == 0
assert tb1.isomorphism(tb2, labelSettings=specLabelSettings) > 0
