include("xxx_helpers.py")

data = """rule [
	left  [
		node [ id 11 label "vL1" ]
		node [ id 12 label "vL2" ]
		edge [ source 11 target 12 label "eL" ]

		node [ id 41 label "vCL1" ]
		node [ id 42 label "vCL2" ]
		edge [ source 41 target 42 label "eCL" ]
	]
	context [
		node [ id 31 label "vK1" ]
		node [ id 32 label "vK2" ]
		edge [ source 31 target 32 label "eK" ]
	]
	right [
		node [ id 21 label "vR1" ]
		node [ id 22 label "vR2" ]
		edge [ source 21 target 22 label "eR" ]

		node [ id 41 label "vCR1" ]
		node [ id 42 label "vCR2" ]
		edge [ source 41 target 42 label "eCR" ]
	]
]"""

r = Rule.fromGMLString(data)
for i in (1, 2, 3, 4):
	globals()["v{}1".format(i)] = r.getVertexFromExternalId(i * 10 + 1)
	globals()["v{}2".format(i)] = r.getVertexFromExternalId(i * 10 + 2)

assert r.numVertices == 8
assert r.numEdges == 4
assert r.left.numVertices == 6
assert r.left.numEdges == 3
assert r.context.numVertices == 4
assert r.context.numEdges == 2
assert r.right.numVertices == 6
assert r.right.numEdges == 3

assert v11.left
assert not v11.right
assert not v11.context
assert v11.left.stringLabel == "vL1"
assert v12.left
assert not v12.right
assert not v12.context
assert v12.left.stringLabel == "vL2"

assert not v21.left
assert v21.right
assert not v21.context
assert v21.right.stringLabel == "vR1"
assert not v22.left
assert v22.right
assert not v22.context
assert v22.right.stringLabel == "vR2"

assert v31.left
assert v31.right
assert v31.context
assert v31.left.stringLabel == "vK1"
assert v31.right.stringLabel == "vK1"
assert v32.left
assert v32.right
assert v32.context
assert v32.left.stringLabel == "vK2"
assert v32.right.stringLabel == "vK2"

assert v41.left
assert v41.right
assert v41.context
assert v41.left.stringLabel == "vCL1"
assert v41.right.stringLabel == "vCR1"
assert v42.left
assert v42.right
assert v42.context
assert v42.left.stringLabel == "vCL2"
assert v42.right.stringLabel == "vCR2"

commonChecks(r)
