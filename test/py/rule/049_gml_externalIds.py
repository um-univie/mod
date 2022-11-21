include("xxx_helpers.py")

def check(a):
	data = {42: "C", 1337: "O", 0: "U"}
	for i in range(a.minExternalId, a.maxExternalId + 1):
		v = a.getVertexFromExternalId(i)
		if i in data:
			assert not v.isNull()
			assert v.left.stringLabel == data[i]
		else:
			assert v.isNull()
	for i, s in data.items():
		v = a.getVertexFromExternalId(i)
		assert not v.isNull()
		assert v.left.stringLabel == data[i]
		

# GML
a = Rule.fromGMLString("""rule [ context [
	node [ id 42 label "C" ]
	node [ id 1337 label "O" ]
	node [ id 0 label "U" ]
	edge [ source 42 target 1337 label "-" ]
	edge [ source 1337 target 0 label "-" ]
] ]""")
check(a)

commonChecks(a)
