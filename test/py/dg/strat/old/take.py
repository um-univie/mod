include("../../../formoseCommon/grammar.py")
include("common.py")

print("===> API")
handleDG(
	addSubset(inputGraphs) >> repeat[2](inputRules)
	>> DGStrat.makeTake(False, 1)
	>> DGStrat.makeTake(True, 0)
)

print("===> nice")
handleDG(
	addSubset(inputGraphs) >> repeat[2](inputRules)
	>> takeSubset(1)
	>> takeUniverse(0)
)
