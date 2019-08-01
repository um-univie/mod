include("../formoseCommon/grammar.py")
include("common.py")

config.dg.listUniverse = True
config.dg.calculateVerbose = True

print("==> explicit call, explicit list")
handleDG(DGStrat.makeParallel([
	dgStrat(inputRules[0]),
	dgStrat(inputRules[1])
	]))

print("==> rule list")
handleDG(inputRules)

print("==> rule set")
handleDG({inputRules[0], inputRules[1]})
