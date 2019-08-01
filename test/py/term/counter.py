config.io.useOpenBabelCoords = False
a = graphDFS("[f(0)][sd(0)]")
incFirst = ruleGMLString("""rule [
	ruleID "Inc first"
	left [
		node [ id 0 label "f(_X)" ]
	]
	right [
		node [ id 0 label "f(s(_X))" ]
	]
]""")
incSecond = ruleGMLString("""rule [
	ruleID "Inc second"
	left [
		node [ id 0 label "sd(_X)" ]
	]
	context [
		node [ id 1 label "f(s(0))" ]
		edge [ source 0 target 1 label "-" ]
	]
	right [
		node [ id 0 label "sd(s(_X))" ]
	]
]""")
incSecond.printTermState()

dg = dgRuleComp(inputGraphs,
	addSubset(a)
	>> repeat[1]([incSecond, incFirst])
	>> incSecond,
	labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification)
)
dg.calc()
dg.print()
