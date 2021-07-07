include("common.py")

fail(lambda: Graph.fromSMILES("[C:1].[O:2]"), "not connected",
	err=InputError, isSubstring=True)
gs = Graph.fromSMILESMulti("[C:1].[O:2]")
assert len(gs) == 2
v1_0 = gs[0].getVertexFromExternalId(1)
assert v1_0.stringLabel == "C"
v2_0 = gs[0].getVertexFromExternalId(2)
assert not v2_0
v1_1 = gs[1].getVertexFromExternalId(1)
assert not v1_1
v2_1 = gs[1].getVertexFromExternalId(2)
assert v2_1.stringLabel == "O"

Graph.fromSMILES("C1C.CC1")
gs = Graph.fromSMILESMulti("C1C.CC1")
assert len(gs) == 1

fail(lambda: Graph.fromSMILES("C.1CCCC.1"),
	"Error in graph loading from smiles string",
	err=InputError, isSubstring=True)
