a = smiles("O")
b = smiles("O")

def fail1(f):
	try:
		print("Trying")
		dg = f()
		assert False
	except LogicError as e:
		print("\t%s" % str(e))
def fail2(f):
	try:
		print("Trying")
		dg = f()
		print("\tCreated")
		dg.calc()
		assert False
	except LogicError as e:
		print("\t%s" % str(e))

fail1(lambda: dgRuleComp([a, b], filterSubset(False)))
fail1(lambda: dgRuleComp([a], addSubset(b)))
fail1(lambda: dgRuleComp([], addSubset(a) >> addSubset(b)))

fail2(lambda: dgRuleComp([a], addSubset(lambda: [b])))
fail2(lambda: dgRuleComp([], addSubset(a) >> addSubset(lambda: [b])))
fail2(lambda: dgRuleComp([], addSubset(lambda: [a]) >> addSubset(lambda: [b])))

dgRuleComp([a, a], filterSubset(False))
dgRuleComp([a], addSubset(a))
dgRuleComp([], addSubset(a) >> addSubset(a))

dgRuleComp([a], addSubset(lambda: [a])).calc()
dgRuleComp([], addSubset(a) >> addSubset(lambda: [a])).calc()
dgRuleComp([], addSubset(lambda: [a]) >> addSubset(lambda: [a])).calc()
