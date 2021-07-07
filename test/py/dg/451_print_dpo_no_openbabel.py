include("../xxx_helpers.py")
post("enableSummary")

config.io.useOpenBabelCoords = False

e = DGHyperEdge()
fail(lambda: e.print(), "Can not print null edge.")

dg = DG()
dg.build().addAbstract("A -> B")
e = next(iter(dg.edges))
fail(lambda: e.print(), "The edge has no rules.")

r = ruleGMLString("""rule [
	left [ node [ id 1 label "B" ] ]
	context [
		node [ id 0 label "A" ]
		edge [ source 0 target 1 label "-" ]
	]
	right [ node [ id 1 label "Q" ] ]
]""")
g = graphDFS("[A][B][C]")
gFail = smiles("O")

dg = DG() 
with dg.build() as b:
	es = b.apply([g], r)
	assert len(es) == 1
	e = es[0]

	d = Derivation()
	d.left = [gFail]
	d.right = [gFail]
	d.rule = r
	eFail = b.addDerivation(d)
	
fail(lambda: eFail.print(), "No derivation exists for rule {}.".format(r.name))

e.print()
p = GraphPrinter()
p.withIndex = True
e.print(printer=p)
e.print(nomatchColour="yellow")
e.print(matchColour="red")
e.print(nomatchColour="yellow", matchColour="red")
