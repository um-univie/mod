rStr = """rule [
	context [
		node [ id 0 label "s" ]
	]
	constrainAdj [
		id 0 op "=" count 0
	]
]"""
r = ruleGMLString(rStr)
try:
	r.makeInverse()
	assert False
except LogicError:
	pass
try:
	ruleGMLString(rStr, invert=True)
	assert False
except InputError:
	pass
config.rule.ignoreConstraintsDuringInversion = True
r.makeInverse()
ruleGMLString(rStr, invert=True)
