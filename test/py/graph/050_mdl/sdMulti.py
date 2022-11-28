include("../../xxx_helpers.py")

mol = """\n\n\n  1  0  0  0  0  0  0  0  0  0999 V3000
M  V30 BEGIN CTAB
M  V30 COUNTS 1 0 0 0 0
M  V30 BEGIN ATOM
M  V30 1 N 0 0 0 0
M  V30 END ATOM
M  V30 END CTAB
M  END
"""
molDis = """\n\n\n  1  0  0  0  0  0  0  0  0  0999 V3000
M  V30 BEGIN CTAB
M  V30 COUNTS 2 0 0 0 0
M  V30 BEGIN ATOM
M  V30 1 C 0 0 0 0
M  V30 2 O 0 0 0 0
M  V30 END ATOM
M  V30 END CTAB
M  END
"""

dc2 = molDis + "$$$$\n"
dc1c2 = mol + "$$$$\n" + molDis + "$$$$\n"

fail(lambda: Graph.fromSDString(dc2), "not connected",
	err=InputError, isSubstring=True)
fail(lambda: Graph.fromSDString(dc1c2),
	"not connected", err=InputError, isSubstring=True)

gc2 = Graph.fromSDStringMulti(dc2)
assert len(gc2) == 1
assert len(gc2[0]) == 2
gc1c2 = Graph.fromSDStringMulti(dc1c2)
assert len(gc1c2) == 2
assert len(gc1c2[0]) == 1
assert len(gc1c2[1]) == 2
