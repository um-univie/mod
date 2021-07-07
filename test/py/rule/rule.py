include("xxx_helpers.py")

testRule = ruleGMLString("""
	rule [ ruleID "inline rule" context [ node [ id 0 label "A" ] ] ]
""")
testRuleInverse = testRule.makeInverse()
ruleGML("rule.gml")
a = ruleGML("rule.gml")
print("a:\t", a)
print("inputRules:\t", inputRules)
with open("myRule.gml", "w") as f:
	f.write(a.getGMLString())
a = ruleGML(CWDPath("myRule.gml"))

try:
	b = a.makeInverse()
except LogicError:
	pass
else:
	assert False

a.printGML()
print(a.getGMLString())

print("name:\t", a.name)
a.name = "New Name"
print("name:\t", a.name)

print("numLeftComponents:\t", a.numLeftComponents)
print("numRightComponents:\t", a.numRightComponents)

# Check eq operator
inputRules[:] = []
a = ruleGML("rule.gml")
rs = inputRules  # TODO: make rs something comming from C++ to get difference Python object with same ptr 
b = rs[0]
print("a:", a)
print("b:", b)
assert a == b
assert a.isomorphism(b) > 0
assert a.isomorphism(testRule) == 0

a = ruleGMLString("""rule [ context [ node [ id 0 label "C" ] ] ]""")


r = ruleGMLString("""rule [
	context [
		node [ id 0 label "*" ]
		node [ id 1 label "*" ]
		edge [ source 0 target 1 label "*" ]
	]
]""")
r.printTermState()
