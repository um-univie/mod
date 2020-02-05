include("../../../formoseCommon/grammar.py")
include("common.py")

def exeFunc(gs):
	print(gs)
print("===> API")
handleDG(addSubset(inputGraphs) >> DGStrat.makeExecute(exeFunc)
	>> filterSubset(lambda g1, gs, first: g1.numVertices < 5)
	>> DGStrat.makeExecute(exeFunc)
)
