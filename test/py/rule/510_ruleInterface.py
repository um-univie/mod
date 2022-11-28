include("../xxx_graphInterface.py")
include("xxx_helpers.py")
import math

def _getOrNan(f):
	try:
		return f()
	except LogicError:
		return float('nan')

def checkCore(a):
	print("Core")
	print("-" * 80)
	print("numVertices:", a.numVertices)
	for v in a.vertices:
		print("Vertex:", v.rule, v.id, v.degree)
		xn = _getOrNan(lambda: v.get2DX(False))
		yn = _getOrNan(lambda: v.get2DY(False))
		xh = _getOrNan(lambda: v.get2DX(True))
		yh = _getOrNan(lambda: v.get2DY(True))
		x = _getOrNan(lambda: v.get2DX())
		y = _getOrNan(lambda: v.get2DY())
		assert math.isnan(xn) == math.isnan(yn)
		assert math.isnan(xh) == math.isnan(yh)
		assert math.isnan(x) == math.isnan(y)

		print("  coords without hydrogens: x={}, y={}".format(xn, yn))
		print("  coords with hydrogens: x={}, y={}".format(xh, yh))
		assert math.isnan(x) == math.isnan(xh)
		assert math.isnan(y) == math.isnan(yh)
		assert math.isnan(x) or x == xh, (x, xh)
		assert math.isnan(y) or y == yh, (y, yh)

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


def checkSide(a, withLabels, side):
	print(side)
	print("-" * 80)
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
			try:
				b = str(e.bondType)
			except LogicError:
				b = "I"
			print("Edge:", e.rule, e.source.id, e.target.id, e.stringLabel, b)
		else:
			print("Edge:", e.rule, e.source.id, e.target.id)
		print("Core:", e.core)


def checkRule(name, a):
	print("#" * 80)
	print(name)
	print("#" * 80)
	a.name = name

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

	checkCore(a)
	checkSide(a.left, True, "L")
	checkSide(a.context, False, "K")
	checkSide(a.right, True, "R")

	commonChecks(a)


def checkGML(name, gml):
	return checkRule(name, Rule.fromGMLString("rule [ {} ]".format(gml)))

def checkDFS(name, dfs):
	return checkRule(name, Rule.fromDFS(dfs))


checkGML("L only", """left [ node [ id 0 label "C" ] ]""")

checkGML("orig example", """
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
	]""")

# K
checkDFS("K, vertex change", "[x]1{e}[y]2>>[a]1{e}[b]2")
checkDFS("K, vertex and edge change", "[x]1{q}[y]2>>[a]1{r}[b]2")
checkDFS("K, chemical vertex change", "[C]1-[S]2>>[N]1-[O]2")
checkDFS("K, chemical vertex and edge change", "[C]1=[S]2>>[N]1#[O]2")

# L
checkDFS("L", "[x]{q}[y]>>")
checkDFS("L, chemical", "[C]=[H]>>")

# R
checkDFS("R", ">>[x]{q}[y]")
checkDFS("R, chemical", ">>[C]=[H]")

# Combinations K
# offset a side to provoke index out of bounds problems
checkDFS("Combinations K, offset L, vertex change",
	"[ox]{oq}[oy].[x]1{e}[y]2>>[a]1{e}[b]2")
checkDFS("Combinations K, offset L, vertex and edge change",
	"[ox]{oq}[oy].[x]1{q}[y]2>>[a]1{r}[b]2")
checkDFS("Combinations K, offset L, chemical vertex chagne",
	"[ox]{oq}[oy].[C]1-[S]2>>[N]1-[O]2")
checkDFS("Combinations K, offset L, chemical vertex and edge change",
	"[ox]{oq}[oy].[C]1=[S]2>>[N]1#[O]2")
checkDFS("Combinations K, offset R, vertex change",
	"[x]1{e}[y]2>>[ox]{oq}[oy].[a]1{e}[b]2")
checkDFS("Combinations K, offset R, vertex and edge change",
	"[x]1{q}[y]2>>[ox]{oq}[oy].[a]1{r}[b]2")
checkDFS("Combinations K, offset R, chemical vertex change",
	"[C]1-[S]2>>[ox]{oq}[oy].[N]1-[O]2")
checkDFS("Combinations K, offset R, chemical vertex and edge change",
	"[C]1=[S]2>>[ox]{oq}[oy].[N]1#[O]2")

# lots of hydrogens, to check coord difference
checkDFS("Hydrogens, coords",
	"{0}>>{0}".format("[C]1([H]2)([H]3)([H]4)[C]5([H]6)([H]7)[C]8([H]9)([H]10)([H]11)"))


# Some structural checks
a = Rule.fromGMLString("""rule [
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

commonChecks(a)


# Some ID checks
a = Rule.fromGMLString("""rule [
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

commonChecks(a)
