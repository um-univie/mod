include("../formoseCommon/grammar.py")
dg = dgRuleComp(inputGraphs,
	addSubset(inputGraphs)
	>> rightPredicate[lambda d: all(g.vLabelCount("C") <= 4 for g in d.right)](
		repeat(inputRules)
	)
)
dg.calc()
dg.print()
for e in dg.edges:
	e.print()
