sys.exit(0)
include("../formoseCommon/grammar.py")

boltz = Boltzmann(40)
def doStuff(g, gs, first):
	print(boltz(g, gs, first))
	return True

dg = dgRuleComp(inputGraphs, addSubset(formaldehyde, glycolaldehyde)
	>> repeat(
		rightPredicate[
			lambda d: all(a.vLabelCount("C") <= 4 for a in d.right)
		](inputRules)
		>> filterUniverse(lambda g, gs, first: doStuff(g, gs.universe, first))
	)
)
dg.calc()
