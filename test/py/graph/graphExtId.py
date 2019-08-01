def check(a):
	data = {42: "C", 1337: "O", 0: "U"}
	for i in range(a.minExternalId, a.maxExternalId + 1):
		v = a.getVertexFromExternalId(i)
		if i in data:
			assert not v.isNull()
			assert v.stringLabel == data[i]
		else:
			assert v.isNull()
	for i, s in data.items():
		v = a.getVertexFromExternalId(i)
		assert not v.isNull()
		assert v.stringLabel == data[i]
		

# GML
a = graphGMLString("""graph [
	node [ id 42 label "C" ]
	node [ id 1337 label "O" ]
	node [ id 0 label "U" ]
	edge [ source 42 target 1337 label "-" ]
	edge [ source 1337 target 0 label "-" ]
]""")
check(a)

# DFS
a = graphDFS("C42O1337[U]0")
check(a)

# SMILES
a = smiles("[C:42][O:1337][U:0]")
check(a)
a = smiles("[C:0][C:0][C:1]")
v = a.getVertexFromExternalId(1)
assert v.isNull()
