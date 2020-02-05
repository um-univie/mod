include("../../formoseCommon/grammar.py")

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs)
	>> rightPredicate[lambda d: all(a.vLabelCount("C") <= 4 for a in d.right)](
		repeat(inputRules)
	)
)
dg.calc()

postSection("Vertex Graphs")
for v in dg.vertices:
	v.graph.print()

printer = DGPrinter()
postSection("Complete")
dg.print(printer)



dg = DG()
dg.build().addAbstract("2 A -> 3 B")
dg.print()
