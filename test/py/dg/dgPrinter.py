include("../formoseCommon/grammar.py")

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs)
	>> rightPredicate[lambda d: all(a.vLabelCount("C") <= 4 for a in d.right)](
		repeat(inputRules)
	)
)
dg.calc()

printer = DGPrinter()
printer.withShortcutEdges = False
printer.withGraphImages = False
printer.labelsAsLatexMath = False
dg.print(printer)

printer = DGPrinter()
printer.vertexLabelSep = "\circ_V{}"
printer.edgeLabelSep = "\circ_E{}"
printer.pushVertexLabel(lambda g, dg: "g_{" + str(g.id) + "}")
printer.pushEdgeLabel(lambda e: "name=" + next(iter(e.rules)).name)

dg.print(printer)

printer.popVertexLabel()
printer.popEdgeLabel()
printer.pushVertexVisible(lambda g, dg: g.vLabelCount("C") != 1)
printer.pushEdgeVisible(lambda e: False if e.numTargets == 2 and e.dg.findVertex(formaldehyde) not in e.targets else True)
printer.withGraphName = False
printer.withRuleName = True
printer.withRuleId = False
dg.print(printer)
printer.withShortcutEdgesAfterVisibility = True
dg.print(printer)
printer.popVertexVisible()
printer.popEdgeVisible()

printer = DGPrinter()
printer.pushVertexColour(lambda g, dg: "Green" if g == formaldehyde else "", True)
printer.pushVertexColour(lambda g, dg: "Blue" if g.vLabelCount("C") == 4 else "", False)
printer.pushVertexColour(lambda g, dg: "Purple" if g.vLabelCount("C") % 2 == 0 else "", True)
printer.pushEdgeColour(lambda e: "Red" if any(v.graph.vLabelCount("C") == 3 for v in e.sources) else "")
dg.print(printer)
printer.popVertexColour()
printer.popVertexColour()
printer.popVertexColour()
printer.popEdgeColour()

printer = DGPrinter()
dg = dgAbstract("2 A -> B")
dg.print(printer)
dg = dgAbstract("3 A -> B")
dg.print(printer)
dg = dgAbstract("4 A -> B")
dg.print(printer)
dg = dgAbstract("5 A -> B")
dg.print(printer)
