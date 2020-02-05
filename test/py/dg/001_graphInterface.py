include("xx0_helpers.py")

dg =  DG()
d1 = Derivations()
d1.left = [smiles("OCC=O")]
d1.right = [smiles("OC=CO")]
d2 = Derivations()
d2.left = d1.right
d2.right = d1.left
with dg.build() as b:
	e1 = b.addDerivation(d1)
	assert dg.numVertices == 2
	assert dg.numEdges == 1
	fail(lambda: e1.inverse, "Can not get inverse edge before the DG is locked.")

	e2 = b.addDerivation(d2)
	assert dg.numVertices == 2
	assert dg.numEdges == 2
	fail(lambda: e2.inverse, "Can not get inverse edge before the DG is locked.")

assert e1.inverse == e2
assert e2.inverse == e1
