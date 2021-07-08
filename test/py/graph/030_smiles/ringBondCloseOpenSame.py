include("common.py")
a = smiles("C1CCCP11NNNN1")
b = graphDFS("C1CCCP2(1)NNNN2")
assert a.isomorphism(b) == 1
