graphDFS("[_x]{_x}[C]")
ruleGMLString("""rule [
	left [
		node [ id 0 label "C" ]
	]
	right [
		node [ id 0 label "U" ]
	]
	constrainAdj [
		id 0 op "=" count 1
		nodeLabels [ label "a" ]
		edgeLabels [ label "b" ]
	]
]""")
dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules,
	labelSettings=LabelSettings(LabelType.Term, LabelRelation.Specialisation))
dg.calc()
dg.print()
