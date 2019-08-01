include("../formoseCommon/grammar.py")
include("common.py")

config.dg.calculatePredicatesVerbose = True

filterFunc = True
print("===> True, API left")
handleDG(addSubset(inputGraphs) >> DGStrat.makeLeftPredicate(filterFunc, dgStrat(inputRules)))
print("===> True, API right")
handleDG(addSubset(inputGraphs) >> DGStrat.makeRightPredicate(filterFunc, dgStrat(inputRules)))

filterFunc = False
print("===> False, API left")
handleDG(addSubset(inputGraphs) >> DGStrat.makeLeftPredicate(filterFunc, dgStrat(inputRules)))
print("===> False, API right")
handleDG(addSubset(inputGraphs) >> DGStrat.makeRightPredicate(filterFunc, dgStrat(inputRules)))

filterFunc = lambda d: len(d.left) == 2
print("===> lambda, API left")
handleDG(addSubset(inputGraphs) >> DGStrat.makeLeftPredicate(filterFunc, dgStrat(inputRules)))
print("===> lambda, API right")
handleDG(addSubset(inputGraphs) >> DGStrat.makeRightPredicate(filterFunc, dgStrat(inputRules)))


print("==> nice left")
handleDG(addSubset(inputGraphs) >> leftPredicate[filterFunc](inputRules))
print("==> nice right")
handleDG(addSubset(inputGraphs) >> rightPredicate[filterFunc](inputRules))
