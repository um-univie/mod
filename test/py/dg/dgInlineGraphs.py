include("../formoseCommon/grammar.py")

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules[0] >> inputRules[2])
dg.calc()


p = DGPrinter()
p.withInlineGraphs = True
dg.print(p)

p = GraphPrinter()
p.setMolDefault()
p.thick = True
for v in dg.vertices:
    v.graph.print(p)
