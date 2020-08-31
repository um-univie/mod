include("xx0_helpers.py")

lsString = LabelSettings(LabelType.String, LabelRelation.Specialisation)
lsTerm = LabelSettings(LabelType.Term, LabelRelation.Specialisation)

def exeStrat(strat, subset=None, universe=None, ls=lsString, graphDatabase=[], **kwargs):
	dg = DG(labelSettings=ls, graphDatabase=graphDatabase)
	b = dg.build()
	res = b.execute(strat, **kwargs)
	res.list(withUniverse=True)
	if subset is not None:
		subset = list(sorted(subset))
		rSubset = list(sorted(res.subset))
		if subset != rSubset:
			print("Subset:      ", subset)
			print("Res subset:  ", rSubset)
			assert False
	if universe is not None:
		universe = list(sorted(universe))
		rUniverse = list(sorted(res.universe))
		if universe != rUniverse:
			print("Universe:    ", universe)
			print("Res universe:", rUniverse)
			assert False
	return dg, b, res
