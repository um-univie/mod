include("../formoseCommon/grammar_H.py")

for a in inputRules: a.print()

config.rc.printMatches = True

def handleExp(exp, printRules=False):
	rc = rcEvaluator(inputRules)
	e = rcExp(exp)
	print("Expression:\t", e)
	res = rc.eval(e)
	print("Result:\t", res)
	print("Database:\t", rc.ruleDatabase)
	print("Products:\t", rc.products)
	if printRules:
		for a in rc.ruleDatabase: a.print()
	postSection("Products")
	for a in rc.products: a.print()
	rc.print()
	inputRules.extend(a for a in rc.products)
