include("xx0_helpers.py")

def doStuff(s, numVertices, numEdges):
	dg = DG()
	dg.build().addAbstract(s)
	names = {}
	for v in dg.vertices:
		assert v.graph.name not in names
		names[v.graph.name] = v
	if numVertices is not None:
		assert dg.numVertices == numVertices
		assert len(names) == numVertices
		concat = ''.join(sorted(names))
		expect = ''.join(chr(ord('a') + i) for i in range(numVertices))	
		assert expect == concat
	assert dg.numEdges == numEdges
	return dg, names
		

doStuff("a -> b", 2, 1)
doStuff("a <=> b", 2, 2)
doStuff("a + b -> a", 2, 1)
doStuff("2 a + b -> 3 c", 3, 1)
doStuff("a -> b b -> d + a d -> c c -> b", 4, 4)
# test  null reactions
doStuff("a -> b b + c -> b + c c + d <=> c + d d -> e", 5, 4)
# test coefficient stuff
dg, names = doStuff("2a -> 2 b", None, 1)
assert dg.numVertices == 2
assert list(sorted(names)) == ['2a', 'b']

msg = "Could not parse description of abstract derivations."
fail(lambda: DG().build().addAbstract(""), msg, err=InputError, isSubstring=True)
fail(lambda: DG().build().addAbstract("\x80"), msg, err=InputError, isSubstring=True)
fail(lambda: DG().build().addAbstract("42\x80"), msg, err=InputError, isSubstring=True)
fail(lambda: DG().build().addAbstract("BrO3- + Br- + H+ + H+ —> HBrO2 + HOBr"), "Expected -> or <=>.", err=InputError)
