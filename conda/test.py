g = smiles('C')
r = ruleGMLString("""rule [
	left [
		node [ id 1 label "C" ]
		node [ id 2 label "H" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 1 label "C-" ]
		node [ id 2 label "H+" ]
	]
]""")
dg = DG()
dg.build().execute(addSubset(g) >> r)

g.print()
r.print()
dg.print()
