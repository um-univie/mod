include("../formoseCommon/grammar.py")
include("common.py")

print("==> explicit call, bound, simple")
handleDG(DGStrat.makeRepeat(1, addSubset(inputGraphs)))

print("==> bound, simple")
handleDG(repeat[1](addSubset(inputGraphs)))

print("==> no bound, simple")
handleDG(repeat(addSubset(inputGraphs)))
