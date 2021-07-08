include("../xxx_graphInterface.py")

a = ruleGMLString("""rule [
	left [
		node [ id 0 label "C+." ]
		edge [ source 100 target 0 label "-" ]
	]
	context [
		node [ id 2 label "H-." ]
		edge [ source 100 target 2 label ":" ]
		node [ id 100 label "U" ]
	]
	right [
		node [ id 1 label "O-." ]
		edge [ source 100 target 1 label "=" ]
	]
]""")


checkGraph(a, string="'{}'".format(a.name),
	vertexString="RuleVertex", edgeString="RuleEdge")
checkLabelledGraph(a.left, string="RuleLeftGraph('{}')".format(a.name),
	vertexString="RuleLeftGraphVertex", edgeString="RuleLeftGraphEdge",
	graphNameInElements=str(a), vIdFull=False)
checkGraph(a.context, string="RuleContextGraph('{}')".format(a.name),
	vertexString="RuleContextGraphVertex", edgeString="RuleContextGraphEdge",
	graphNameInElements=str(a), vIdFull=False)
checkLabelledGraph(a.right, string="RuleRightGraph('{}')".format(a.name),
	vertexString="RuleRightGraphVertex", edgeString="RuleRightGraphEdge",
	graphNameInElements=str(a), vIdFull=False)




print("Core\n" + "="*80)
print("numVertices:", a.numVertices)
for v in a.vertices:
	print("Vertex:", v.rule, v.id, v.degree)
	if v.left.isNull():
		print("\tLeft: N/A")
	else:
		print("\tLeft:", v.left, v.left.stringLabel)
		assert v.left.core == v
	if v.context.isNull():
		print("\tContext: N/A")
	else:
		print("\tContext:", v.context)
		assert v.context.core == v
	if v.right.isNull():
		print("\tRight: N/A")
	else:
		print("\tRight:", v.right, v.right.stringLabel)
		assert v.right.core == v
	for e in v.incidentEdges:
		print("\tEdge:", e.target.id)
print("numEdges:", a.numEdges)
for e in a.edges:
	print("Edge:", e.rule, e.source.id, e.target.id)
	if e.left.isNull():
		print("Left: N/A")
	else:
		print("Left:", e.left)
		assert e.left.core == e
	if e.context.isNull():
		print("Context: N/A")
	else:
		print("Context:", e.context)
		assert e.context.core == e
	if e.right.isNull():
		print("Right: N/A")
	else:
		print("Right:", e.right)
		assert e.right.core == e


def checkGraph(a, withLabels, name):
	print(name + "\n" + "="*80)
	print("rule:", a.rule.name)
	print("numVertices:", a.numVertices)
	for v in a.vertices:
		if withLabels:
			print("Vertex:", v.rule, v.id, v.degree, v.stringLabel, v.atomId, v.isotope, v.charge, v.radical)
		else:
			print("Vertex:", v.rule, v.id, v.degree)
		assert v.id == v.core.id
		for e in v.incidentEdges:
			print("\tEdge:", e.target.id)
	print("numEdges:", a.numEdges)
	for e in a.edges:
		if withLabels:
			print("Edge:", e.rule, e.source.id, e.target.id, e.stringLabel, e.bondType)
		else:
			print("Edge:", e.rule, e.source.id, e.target.id)
		print("Core:", e.core)

checkGraph(a.left, True, "Left")
checkGraph(a.context, False, "Context")
checkGraph(a.right, True, "Right")



a = ruleGMLString("""rule [
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
		edge [ source 0 target 1 label "-" ]
	]
]""")
v0 = a.vertices[0]
v1 = a.vertices[1]
assert v0 == v0
assert v0 != v1
e1 = list(v0.incidentEdges)[0]
e2 = list(v1.incidentEdges)[0]
assert e1 == e2
assert not e1 != e2



a = ruleGMLString("""rule [
	left [
		node [ id 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 2 label "C" ]
	]
]""")
v0 = a.getVertexFromExternalId(0)
v1 = a.getVertexFromExternalId(1)
v2 = a.getVertexFromExternalId(2)
assert v0.id == 0, v0.id
assert v1.id == 1, v1.id
assert v2.id == 2, v2.id

assert v0.left.id    == 0, v0.left.id
assert v0.context.id == 0, v0.context.id
assert v0.right.id   == 0, v0.right.id
assert v1.left.id    == 1, v1.left.id
assert v2.right.id   == 2, v2.right.id

print(v0.left.stringLabel, v0.right.stringLabel)
print(v1.left.stringLabel)
print(v2.right.stringLabel)
