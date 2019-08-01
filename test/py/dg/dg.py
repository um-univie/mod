config.dg.printVertexIds = True
include("../formoseCommon/grammar.py")

config.dg.listUniverse = True
config.dg.printVertexIds = True

try:
	dgDump(inputGraphs, inputRules, "nope.dg")
	assert False
except mod.InputError:
	pass

dg = dgDump(inputGraphs, inputRules, "DGDump.dg")
print("Dump:\t", dg)

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs))
print("RuleComp:\t", dg)
dg.calc()
print("GraphDatabase:\t", dg.graphDatabase)
print("Products:\t", dg.products)
f = dg.print()
print("print file:", f)

dg.dump()

dg = dgRuleComp(inputGraphs, filterSubset(False))
try:
    dg.print()
    assert False
except LogicError:
    pass


dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules[0] >> inputRules[2])
dg.calc()
dg.list()

educts = [glycolaldehyde]
products = dg.products[0:1]
eductVertices = [dg.findVertex(a) for a in educts]
productVertices = [dg.findVertex(a) for a in products]
e = dg.findEdge(eductVertices, productVertices)
assert not e.isNull()
print(e)
e = dg.findEdge(educts, products)
assert not e.isNull()
print(e)
dg.list()
dg.listStats()

dumpFirst = dg.dump()
dg = dgDump(dg.graphDatabase, inputRules, CWDPath(dumpFirst))
dumpSecond = dg.dump()
import filecmp
print("dumpFirst: %s, dumpSecond: %s" % (dumpFirst, dumpSecond))
assert filecmp.cmp(dumpFirst, dumpSecond)

postSection("Complete")
dg.print()

ders = []
for e in dg.edges:
    if len(e.rules) == 0:
        d = Derivation()
        d.left = [a.graph for a in e.sources]
        d.right = [a.graph for a in e.dg.targets]
        ders.append(d)
    else:
        for r in e.rules:
            d = Derivation()
            d.left = [a.graph for a in e.sources]
            d.right = [a.graph for a in e.targets]
            d.rule = r
            ders.append(d)
dg = dgDerivations(ders)
dg.print()

postSection("Non")
printer = DGPrinter()
printer.pushVertexVisible(False)
dg.print(printer)

postSection("Not Input")
printer = DGPrinter()
printer.pushVertexVisible(lambda g, dg: all(g != a for a in inputGraphs))
dg.print(printer)

postSection("Highlight")
printer = DGPrinter()
printer.pushVertexColour(lambda g, dg: "Blue" if g == formaldehyde else "")
dg.print(printer)

postSection("Properties")
printer.pushVertexLabel(lambda g, dg: "|V|=" + str(g.numVertices))
printer.pushEdgeLabel(lambda d: "d=" + str(d))
dg.print(printer)
printer.graphPrinter.withIndex = True
dg.print(printer)


print("numVertices:", dg.numVertices)
for v in dg.vertices:
	assert v.dg == dg
	print(v.id, v.inDegree, v.outDegree, v.graph)
	print("\tin:", end="")
	for e in v.inEdges:
		print("", e.id, end="")
	print()
	print("\tout:", end="")
	for e in v.outEdges:
		print("", e.id, end="")
	print()
	assert dg.findVertex(v.graph) == v
print("numEdges:", dg.numEdges)
for e in dg.edges:
	assert e.dg == dg
	print(e.id, e.numSources, e.numTargets, "{", end="")
	for s in e.sources:
		print("", s.id, end="")
	print(" }, {", end="")
	for r in e.rules:
		print("", str(r), end="")
	print(" }, {", end="")
	for t in e.targets:
		print("", t.id, end="")
	print(" }", end="")
	print("  ", e.inverse)
	e.print()
	e.printTransitionState()


# TODO: test the __eq__ operator
