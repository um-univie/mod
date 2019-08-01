a = graphDFS("[a]")
aa = graphDFS("[_A][_A]")
aaWild = graphDFS("[*]{*}[f(*)]")

p = ruleGMLString("""rule [
	ruleID "A"
	left [
		node [ id 0 label "a" ]
	]
	right [
		node [ id 0 label "_X" ]
		node [ id 1 label "_X" ]
		edge [ source 0 target 1 label "-" ]
	]
]""")
pWild = ruleGMLString("""rule [
	ruleID "AWild"
	left [
		node [ id 0 label "a" ]
	]
	right [
		node [ id 0 label "*" ]
		node [ id 1 label "f(*)" ]
		edge [ source 0 target 1 label "*" ]
	]
]""")
	

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules, labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification))
dg.calc()
dg.print()
postSection("Input Graphs")
for a in inputGraphs:
	a.print()
	a.printTermState()
postSection("Input Rules")
for a in inputRules:
	a.print()
	a.printTermState()
postSection("Vertex Graphs")
for a in set((v.graph for v in dg.vertices)) - set(inputGraphs):
	a.print()
	a.printTermState()
postSection("Derivations")
for e in dg.edges:
	e.print()
