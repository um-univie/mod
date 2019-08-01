ls = LabelSettings(LabelType.Term, LabelRelation.Unification)
def fail(f):
	try:
		f()
		assert False
	except TermParsingError as e:
		print(e)

g = graphDFS("[C/][C]")
r = ruleGMLString("""rule [
	context [
		node [ id 0 label "C/" ]
	]
]""")

for a in inputGraphs:
	fail(lambda: a.printTermState())
for a in inputRules:
	fail(lambda: a.printTermState())

fail(lambda: dgRuleComp(inputGraphs, addSubset(lambda: []), ls))
fail(lambda: dgRuleComp([], addSubset(inputGraphs), ls))
dg = dgRuleComp([], addSubset(lambda: inputGraphs), ls)
fail(lambda: dg.calc())
dg = dgRuleComp([], inputRules, ls)
fail(lambda: dg.calc())
fail(lambda: g.isomorphism(g, 1, ls))
fail(lambda: g.monomorphism(g, 1, ls))
fail(lambda: r.isomorphism(r, 1, ls))
fail(lambda: r.monomorphism(r, 1, ls))
fail(lambda: rcEvaluator(inputRules, ls))
rc = rcEvaluator([], ls)
fail(lambda: rc.eval(inputRules *rcSuper* inputRules))
