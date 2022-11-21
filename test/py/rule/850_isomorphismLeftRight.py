include("xxx_helpers.py")

rId = Rule.fromGMLString("""rule [
	ruleID "id"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
]""")
rSwap = Rule.fromGMLString("""rule [
	ruleID "id"
	left [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
	right [
		node [ id 0 label "B" ]
		node [ id 1 label "A" ]
	]
]""")
assert rId.isomorphism(rSwap) == 0
assert rId.isomorphicLeftRight(rSwap)

commonChecks(rId)
commonChecks(rSwap)
