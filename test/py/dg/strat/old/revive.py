include("../../../formoseCommon/grammar.py")
include("common.py")

print("==> normal")
handleDG(addSubset(inputGraphs) >> inputRules[0])

print("==> with explicit revive")
handleDG(addSubset(inputGraphs) >> DGStrat.makeRevive(dgStrat(inputRules[0])))

print("==> with implicit revive")
handleDG(addSubset(inputGraphs) >> revive(inputRules[0]))
