ruleGMLString("""rule [
	left [
		node [ id 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 2 label "C" ]
		edge [ source 0 target 1 label "-" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 1 label "Q" ]
	]
]""")
ruleGMLString("""rule [
	left [
		node [ id 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
		edge [ source 0 target 1 label "-" ]
	]
	right [
		node [ id 1 label "Q" ]
	]
]""")
graphDFS("[A][B][C]")

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules)
dg.calc()
for e in dg.edges:
	e.print()
