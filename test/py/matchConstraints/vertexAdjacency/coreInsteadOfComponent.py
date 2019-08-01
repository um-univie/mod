# test case for the bug where the constraint is checked in the core graph
# instead of the component graph
rLeft = ruleGMLString("""rule [
	left [
		edge [ source 1 target 2 label "-" ]
	]
	context [
		node [ id 1 label "C" ]
		node [ id 2 label "O" ]
	]
]""")
rRight = ruleGMLString("""rule [
	context [
		node [ id 1 label "C" ]
	]
	constrainAdj [
		id 1 count 0 op "="
		nodeLabels [ label "O" ]
	]
]""")
rc = rcEvaluator(inputRules)
exp = rc.eval(rLeft *rcSuper(enforceConstraints=True)* rRight)
assert len(exp) > 0
for a in exp: a.print()
