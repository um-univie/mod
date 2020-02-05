def makeGraph(name):
	return graphDFS("[%s]" % name)
def makeRule(name):
	return ruleGMLString('rule [ ruleID "%s" ]' % name)
def makeDer(left, rule, right):
	d = Derivation()
	d.left = left
	d.rule = rule
	d.right = right
	return d

a = makeGraph("a")
b = makeGraph("b")
c = makeGraph("c")
d = makeGraph("d")
r1 = makeRule("r1")
r2 = makeRule("r2")
r3 = makeRule("r3")
r4 = makeRule("r4")

print("DG1")
dg1 = dgDerivations([
	makeDer([a], r1, [b]),
	makeDer([a], r2, [b]),
	makeDer([a, a], r1, [b]),
])
print("DG2")
dg2 = dgDerivations([
	makeDer([a, a], r2, [b]),
	makeDer([a], r1, [b]),
])
diffDGs(dg1, dg2)
dg1.print()
dg2.print()
