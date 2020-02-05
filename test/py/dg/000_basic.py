include("xx0_helpers.py")

g = smiles('O')
v = DGVertex()

dg =  DG()
assert dg.id == 0
assert str(dg) == "DG(%d)" % dg.id
assert str(dg) == repr(dg)

assert not dg.hasActiveBuilder
assert not dg.locked

msg = "The DG neither has an active builder nor is locked yet."
fail(lambda: dg.numVertices, msg)
fail(lambda: dg.vertices, msg)
fail(lambda: dg.numEdges, msg)
fail(lambda: dg.edges, msg)

b1 = dg.build()
# Active builder, not locked
assert not dg.locked
assert dg.hasActiveBuilder

assert dg.numVertices == 0
assert list(dg.vertices) == []
assert dg.numEdges == 0
assert list(dg.edges) == []

fail(lambda: dg.build(), "Another build is already in progress.")

del b1
# Locked, no active builder
fail(lambda: dg.build(), "The DG is locked.")

assert dg.numVertices == 0
assert list(dg.vertices) == []
assert dg.numEdges == 0
assert list(dg.edges) == []


dg = DG()
assert dg.labelSettings.type == LabelType.String
assert dg.labelSettings.relation == LabelRelation.Isomorphism
assert not dg.labelSettings.withStereo
assert dg.labelSettings.stereoRelation == LabelRelation.Isomorphism
ls = LabelSettings(LabelType.Term, LabelRelation.Isomorphism, LabelRelation.Isomorphism)
dg = DG(labelSettings=ls)
assert dg.labelSettings.type == LabelType.Term
assert dg.labelSettings.relation == LabelRelation.Isomorphism
assert dg.labelSettings.withStereo
assert dg.labelSettings.stereoRelation == LabelRelation.Isomorphism
