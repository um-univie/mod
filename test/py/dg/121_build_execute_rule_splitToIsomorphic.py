include("1xx_execute_helpers.py")

rSplit = ruleGMLString("""rule [
	ruleID "Split to isomorphms"
	left [
		edge [ source 1 target 2 label "-" ]
	]
	context [
		node [ id 1 label "Q" ]
		node [ id 2 label "Q" ]
	]
]""")
g1 = graphDFS("[Q][Q]", "QQ")
g2 = graphDFS("[Q]", "Q")
exeStrat(addSubset(g1) >> rSplit, [g2], [g1, g2], graphDatabase=[g1, g2])
