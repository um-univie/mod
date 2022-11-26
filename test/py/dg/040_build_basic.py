include("xx0_helpers.py")

dg = DG()
b = dg.build()
assert b.dg == dg
assert b.isActive
