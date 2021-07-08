include("common.py")
a = smiles("C1C2CCC12(N)(P)")
config.graph.smilesCheckAST = False
b = smiles("C1C2CCC(N)1(P)2")
config.graph.smilesCheckAST = True
assert a.isomorphism(b) > 0
