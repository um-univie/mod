include("common.py")

a = Graph.fromDFS("[C]{.}[C]")
assert a.numVertices == 2
assert a.numEdges == 1
assert next(iter(a.edges)).stringLabel == '.'

fail(lambda: Graph.fromDFS("[C]1.[O]2"), "not connected",
	err=InputError, isSubstring=True)
gs = Graph.fromDFSMulti("[C]1.[O]2")
assert len(gs) == 2
v1_0 = gs[0].getVertexFromExternalId(1)
assert v1_0.stringLabel == "C"
v2_0 = gs[0].getVertexFromExternalId(2)
assert not v2_0
v1_1 = gs[1].getVertexFromExternalId(1)
assert not v1_1
v2_1 = gs[1].getVertexFromExternalId(2)
assert v2_1.stringLabel == "O"

Graph.fromDFS("C1C.CC1")
gs = Graph.fromDFSMulti("C1C.CC1")
assert len(gs) == 1

Graph.fromDFS("C1CCCC.1")
gs = Graph.fromDFSMulti("C1CCCC.1")
assert len(gs) == 1

gs = Graph.fromDFSMulti("[A]1.[B].1[X]")
assert len(gs) == 2
assert gs[0].numVertices == 1
assert next(iter(gs[0].vertices)).stringLabel == "A"
assert gs[1].numVertices == 2
assert tuple(sorted(v.stringLabel for v in gs[1].vertices)) == ("B", "X")
