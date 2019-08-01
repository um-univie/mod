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

a.print()
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
a.print()


inputRules[:] = []
include("../formoseCommon/grammar.py")
postSection("Formose")
for a in inputRules: a.print()
inputRules[0].print(GraphPrinter(), GraphPrinter())

try:
	rBad = ruleGMLString("""rule [
		left  [ node [ id 0 label "A" ] ]
		right [ node [ id 0 label "A" ] ]
	]""")
except InputError as e:
	print("InputError catched")
try:
	rBad = ruleGMLString("""rule [
		left  [ edge [ source 0 target 1 label "A" ] ]
		context [
			node [ id 0 label "A" ]
			node [ id 1 label "A" ]
		]
		right  [ edge [ source 0 target 1 label "A" ] ]
	]""")
except InputError as e:
	print("InputError catched")
r = ruleGMLString("""rule [
	context [
		node [ id 0 label "*" ]
		node [ id 1 label "*" ]
		edge [ source 0 target 1 label "*" ]
	]
]""")
r.printTermState()
