# 'Z' is non-chemical, and is not printed
def printStereo(a):
	for v in a.vertices:
		if v.stringLabel == "Q" or v.atomId != AtomIds.Invalid:
			v.printStereo()
	
def dfs(s):
	a = mod.graphDFS(s)
	printStereo(a)
	return a
def gGML(s):
	a = mod.graphGMLString("graph [ " + s + " ]")
	printStereo(a)
	return a
def rGML(s, side):
	a = mod.ruleGMLString("rule [ %s [ %s ] ]" % (side, s))
	printStereo(a.left)
	printStereo(a.right)
	return a

isoLabelSettings = LabelSettings(
	LabelType.String,
	LabelRelation.Isomorphism,
	LabelRelation.Isomorphism
)
specLabelSettings = LabelSettings(
	LabelType.Term,
	LabelRelation.Specialisation,
	LabelRelation.Specialisation
)
