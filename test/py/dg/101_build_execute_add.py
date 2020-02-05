include("1xx_execute_helpers.py")

g1a = smiles('O', "g1a")
g1b = smiles('O', "g1b")
g2a = smiles('C', "g2a")

# static
# --------------------------------------------------------
exeStrat(DGStrat.makeAddStatic(True, [], IsomorphismPolicy.Check), [], [])
exeStrat(DGStrat.makeAddStatic(False, [], IsomorphismPolicy.Check), [], [])
exeStrat(addSubset([]), [], [])
exeStrat(addUniverse([]), [], [])
exeStrat(addSubset([g1a]), [g1a], [g1a])
exeStrat(addUniverse([g1a]), [], [g1a])
exeStrat(addSubset(g1a), [g1a], [g1a])
exeStrat(addUniverse(g1a), [], [g1a])
exeStrat(addSubset([g1a], [g2a]), [g1a, g2a], [g1a, g2a])
exeStrat(addUniverse([g1a], [g2a]), [], [g1a, g2a])
exeStrat(addSubset(g1a, [g2a]), [g1a, g2a], [g1a, g2a])
exeStrat(addUniverse(g1a, [g2a]), [], [g1a, g2a])
exeStrat(addSubset([g1a], g2a), [g1a, g2a], [g1a, g2a])
exeStrat(addUniverse([g1a], g2a), [], [g1a, g2a])
exeStrat(addSubset(g1a, g2a), [g1a, g2a], [g1a, g2a])
exeStrat(addUniverse(g1a, g2a), [], [g1a, g2a])

# isomorphic
fail(lambda: exeStrat(addSubset(g1a, g1b)),
	"Isomorphic graphs. Candidate graph 'g1b' is isomorphic to 'g1a' in the graph database.")
fail(lambda: exeStrat(addUniverse(g1a, g1b)),
	"Isomorphic graphs. Candidate graph 'g1b' is isomorphic to 'g1a' in the graph database.")
exeStrat(addSubset(g1a, g1b, graphPolicy=IsomorphismPolicy.TrustMe))
exeStrat(addUniverse(g1a, g1b, graphPolicy=IsomorphismPolicy.TrustMe))


# dynamic
# --------------------------------------------------------
exeStrat(DGStrat.makeAddDynamic(True, lambda: [], IsomorphismPolicy.Check), [], [])
exeStrat(DGStrat.makeAddDynamic(False, lambda: [], IsomorphismPolicy.Check), [], [])
exeStrat(addSubset(lambda: []), [], [])
exeStrat(addUniverse(lambda: []), [], [])
exeStrat(addSubset(lambda: [g1a]), [g1a], [g1a])
exeStrat(addUniverse(lambda: [g1a]), [], [g1a])
fail(lambda: exeStrat(addSubset(lambda: [g1a], 42)),
	"The dynamic version of addSubset/addUniverse takes exactly 1 argument (2 given).", err=TypeError)
fail(lambda: exeStrat(addUniverse(lambda: [g1a], 42)),
	"The dynamic version of addSubset/addUniverse takes exactly 1 argument (2 given).", err=TypeError)

# isomorphic
fail(lambda: exeStrat(addSubset(lambda: [g1a, g1b])),
	"Isomorphic graphs. Candidate graph 'g1b' is isomorphic to 'g1a' in the graph database.")
fail(lambda: exeStrat(addUniverse(lambda: [g1a, g1b])),
	"Isomorphic graphs. Candidate graph 'g1b' is isomorphic to 'g1a' in the graph database.")
exeStrat(addSubset(lambda: [g1a, g1b], graphPolicy=IsomorphismPolicy.TrustMe))
exeStrat(addUniverse(lambda: [g1a, g1b], graphPolicy=IsomorphismPolicy.TrustMe))






