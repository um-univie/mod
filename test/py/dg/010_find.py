include("xx0_helpers.py")

g = smiles('O')
v = DGVertex()

dg =  DG()

msg = "The DG neither has an active builder nor is locked yet."
fail(lambda: dg.findVertex(g), msg)
fail(lambda: dg.findEdge([v], [v]), msg)
fail(lambda: dg.findEdge([g], [g]), msg)

def active():
	assert dg.findVertex(g) == DGVertex()
	fail(lambda: dg.findEdge([v], []), "Source vertex descriptor is null.")
	fail(lambda: dg.findEdge([], [v]), "Target vertex descriptor is null.")
	fail(lambda: dg.findEdge([g], []), "Source vertex descriptor is null.")
	fail(lambda: dg.findEdge([], [g]), "Target vertex descriptor is null.")

	fail(lambda: dg.findVertex(None), "The graph is a nullptr.")
	fail(lambda: dg.findEdge([None], [g]), "Incompatible Data Type", err=TypeError)
	fail(lambda: dg.findEdge([g], [None]), "Incompatible Data Type", err=TypeError)
	fail(lambda: dg.findEdge([g, None], [g]), "The graph is a nullptr.")
	fail(lambda: dg.findEdge([g], [g, None]), "The graph is a nullptr.")

b = dg.build()
# Active builder, not locked
active()

del b
# Locked, no active builder
active()
