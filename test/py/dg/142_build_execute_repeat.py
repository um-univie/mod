include("1xx_execute_helpers.py")

c1 = smiles("[C]", "c1")
c2 = smiles("[C][C]", "c2")
c3 = smiles("[C][C][C]", "c3")
n1 = smiles("[N]", "n1")
n2 = smiles("[N][N]", "n2")
n3 = smiles("[N][N][N]", "n3")
c1n1 = smiles("[C][N]", "c1n1")
c2n1e = smiles("[C][C][N]", "c2n1e")
c2n1m = smiles("[C][N][C]", "c2n1m")
c1n2e = smiles("[C][N][N]", "c1n2e")
c1n2m = smiles("[N][C][N]", "c1n2m")


r = ruleGMLString("""rule [
	left  [ node [ id 0 label "C" ] ]
	right [ node [ id 0 label "N" ] ]
]""")

fail(lambda: DGStrat.makeRepeat(-42, None), "Limit must be non-negative.")
DGStrat.makeRepeat(0, addSubset(c1))
fail(lambda: DGStrat.makeRepeat(42, None), "The substrategy is a null pointer.")

exeStrat(addSubset(c1, c2, c3) >> repeat[1](r),
	[n1, c1n1, c2n1e, c2n1m], [c1, c2, c3, n1, c1n1, c2n1e, c2n1m], graphDatabase=inputGraphs)
exeStrat(addSubset(c1, c2, c3) >> repeat[2](r),
	[n2, c1n2e, c1n2m], [c1, c2, c3, n1, n2, c1n1, c2n1e, c2n1m, c1n2e, c1n2m], graphDatabase=inputGraphs)
exeStrat(addSubset(c1, c2, c3) >> repeat[3](r),
	[n3], [c1, c2, c3, n1, n2, n3, c1n1, c2n1e, c2n1m, c1n2e, c1n2m], graphDatabase=inputGraphs)
exeStrat(addSubset(c1, c2, c3) >> repeat(r),
	[n3], [c1, c2, c3, n1, n2, n3, c1n1, c2n1e, c2n1m, c1n2e, c1n2m], graphDatabase=inputGraphs)

exeStrat(addSubset(c1) >> repeat(filterSubset(False)), [c1], [c1])
exeStrat(addSubset(c1) >> repeat(filterSubset(False) >> addSubset(c2)), [c2], [c1, c2])

exeStrat(addSubset(c1) >> repeat[0](addSubset(c2)), [c1], [c1])
