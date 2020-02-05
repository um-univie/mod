include("../../../formoseCommon/grammar.py")
include("common.py")

filterFunc = True
print("===> both, True, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(True, filterFunc))
print("===> subset, True, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(False, filterFunc))

filterFunc = False
print("===> both, False, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(True, filterFunc))
print("===> subset, False, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(False, filterFunc))

filterFunc = lambda g, gs, first: (g.numVertices < 60
		and len(gs.subset) < 40
		and len(gs.universe) < 5
	)
filterFunc_lambda = filterFunc
def filterFunc(g, gs, first):
	print("subset: " + str(gs.subset))
	print("universe: " + str(gs.universe))
	return filterFunc_lambda(g, gs, first)

print("===> both, lambda, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(True, filterFunc))
print("===> subset, lambda, API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeFilter(False, filterFunc))


print("===> both, lambda, nice")
handleDG(addSubset(inputGraphs) >> filterUniverse(filterFunc))
print("===> subset, lambda, nice")
handleDG(addSubset(inputGraphs) >> repeat[3](inputRules >> filterSubset(filterFunc)))
