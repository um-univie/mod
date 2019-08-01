rSplit = ruleGML("splitToIsomorphs.gml")
rSplit.print()
g1 = graphDFS("[Q][Q]")
dg = dgRuleComp(inputGraphs, addSubset(g1) >> rSplit)
config.dg.calculateVerbose = True
dg.calc()
dg.print()
for a in dg.products: a.print()
