include("../../xxx_helpers.py")

dataDis = """\n\n\n  1  0  0  0  0  0  0  0  0  0999 V3000
M  V30 BEGIN CTAB
M  V30 COUNTS 2 0 0 0 0
M  V30 BEGIN ATOM
M  V30 1 C 0 0 0 0
M  V30 2 O 0 0 0 0
M  V30 END ATOM
M  V30 END CTAB
M  END"""
fail(lambda: Graph.fromMOLString(dataDis), "not connected",
	err=InputError, isSubstring=True)
gs = Graph.fromMOLStringMulti(dataDis)
assert len(gs) == 2
