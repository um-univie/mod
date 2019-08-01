config.dg.listUniverse = True
config.dg.calculateVerbose = True

def handleDG(strat):
	dg = dgRuleComp(inputGraphs, strat)
	dg.calc()
	dg.list()
	dg.print()
