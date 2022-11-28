include("common.py")

a1 = Graph.fromDFSMulti("[A] 1 ( [B] ) [C] [D] {E} [F] . [G]")
a2 = Graph.fromDFSMulti("[A]1([B])[C][D]{E}[F].[G]")
assert a1[0].isomorphism(a2[0]) != 0
assert a1[1].isomorphism(a2[1]) != 0
assert len(a1) == 2
assert len(a2) == 2
