a = ruleGMLString("""rule [
	ruleID "a"
	context [
		node [ id 0 label "A" ]
	]
]""")
b = ruleGMLString("""rule [
	ruleID "b"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
]""")
c = ruleGMLString("""rule [
	ruleID "c"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
		node [ id 2 label "C" ]
	]
]""")
rc = rcEvaluator(inputRules)
for i in [a, b, c]:
	for j in [a, b, c]:
		print(i, j)
		rc.eval(i *rcSuper(allowPartial=False)* j)
