def create():
	r = ruleGMLString("""rule [
		context [
			node [ id 0 label "C" ]
		]
	]""", add=False)
	g = smiles("C", add=False)

	d = Derivation()
	d.left = [g]
	d.right = [g]
	d.rule = r
	return d

dg = DG()
with dg.build() as b:
	b.addDerivation(create())
for e in dg.edges:
	e.print()
