a = smiles('O')
d = Derivation()
d.left = [a]
d.right = [a]

dg = DG()
with dg.build() as b:
	b.addDerivation(d)
	e = dg.findEdge([a], [a])
	assert not e.isNull()
	print(e)
