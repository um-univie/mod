def test(a):
	a.print()
	p = GraphPrinter()
	p.setMolDefault()
	p.withIndex = True
	a.print(p)
	print(a.smiles)
	b = smiles(a.smiles)
	assert a.isomorphism(b) > 0
test(graphDFS("[C.]"))
test(graphDFS("[C.]" + "(C)"))
test(graphDFS("[C.]" + "(C)"*2))
test(graphDFS("[C.]" + "(C)"*3))
test(graphDFS("[C.]" + "(C)"*4))
test(graphDFS("[C.]" + "(C)"*5))
test(graphDFS("[C.]" + "(C)"*6))
test(graphDFS("[C.]" + "(C)"*7))
test(smiles("[CH2+.:42]"))
test(smiles("[CH+.:42]"))
