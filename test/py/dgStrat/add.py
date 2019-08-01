include("../formoseCommon/grammar.py")
include("common.py")

print("===> addUniverse(graph)")
handleDG(addUniverse(inputGraphs[0]))

print("===> addUniverse(graphs)")
handleDG(addUniverse(inputGraphs))

print("===> addSubset(inputGraphsraph)")
handleDG(addSubset(inputGraphs[0]))
print("===> addSubset(inputGraphsraphs)")
handleDG(addSubset(inputGraphs))

print("===> addSubset(inputGraphsraph, graphs)")
a = smiles("O", add=False)
handleDG(addSubset(a, inputGraphs))


print("===> addUniverse(lambda)")
handleDG(addUniverse(lambda: [a]))

print("===> addSubset(lambda)")
handleDG(addSubset(lambda: [a]))

dynGraphs = []
def setDynGraphs(gs):
	dynGraphs.append(a)
print("===> add(lambda) dynamic test")
handleDG(addUniverse(lambda: dynGraphs) >> execute(setDynGraphs) >> addUniverse(lambda: dynGraphs))
