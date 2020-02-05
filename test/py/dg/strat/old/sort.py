include("../../../formoseCommon/grammar.py")
include("common.py")

less = lambda g1, g2, gs: g1.numVertices > g2.numVertices
print("===> API")
handleDG(
	addSubset(inputGraphs) >> repeat[2](inputRules)
	>> DGStrat.makeSort(False, less)
	>> DGStrat.makeSort(True, less)
)

print("===> nice")
handleDG(
	addSubset(inputGraphs) >> repeat[2](inputRules)
	>> sortSubset(less)
	>> sortUniverse(less)
)
