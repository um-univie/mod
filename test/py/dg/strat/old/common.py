def handleDG(strat):
	dg = DG(graphDatabase=inputGraphs)
	dg.build().execute(strat, verbosity=8)
	dg.print()
