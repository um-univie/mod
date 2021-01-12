post("disableSummary")

include("rules.py")
#for a in inputRules:
#	a.print()
#	a.printTermState()

rc = 'dummy'
iso = 'dummy'

checkNum = 0
def check(exp, checks, resSize=1):
	global checkNum
	print("Check", checkNum, exp)
	checkNum += 1
	res = list(set(rc.eval(exp)))
	def redo():
		rc.eval(exp, verbosity=20)
	if len(rc.products) != 0:
		postSection("Product Error, Result")
		print("ERROR")
		print("=============================================================")
		for a in res: a.print()
		redo()
		assert False
	if len(res) != resSize:
		postSection("Len Error, Result")
		print("ERROR")
		print("=============================================================")
		for a in res: a.print()
		redo()
		assert False
	for a in checks:
		if not a(res):
			postSection("Check Error, Result")
			print("ERROR")
			print("=============================================================")
			for a in res: a.print()
			redo()
			assert False
rcSub = rcSub(allowPartial=False)
rcSuper = rcSuper(allowPartial=False)
rcCommonMax = rcCommon(maximum=True, connected=False)
rcCommon = rcCommon(maximum=False, connected=False)

def testSettings(settings):
	global rc
	global iso
	global checkNum
	checkNum = 0
	rc = rcEvaluator(inputRules, labelSettings=settings)
	iso = lambda index, rule: lambda res: res[index].isomorphism(rule, labelSettings=settings) > 0
	include("checks.py", checkDup=False, skipDup=False)
	doChecks()
testSettings(LabelSettings(LabelType.String, LabelRelation.Isomorphism))
testSettings(LabelSettings(LabelType.Term, LabelRelation.Isomorphism))
#testSettings(LabelSettings(LabelType.String, LabelRelation.Isomorphism, LabelRelation.Isomorphism))
