print("AtomId\n----------")
a = AtomId()
print(a)
a = AtomId(42)
print(a)
print(int(a))

print("Charge\n----------")
a = Charge()
print(a)
a = Charge(-42)
print(a)
print(int(a))

print("AtomData\n----------")
a = AtomData()
print(a.atomId)
print(a.isotope)
print(a.charge)
print(a.radical)
a = AtomData(AtomId(42), Isotope(60), Charge(-9), True)
print("AtomData:", a)
b = AtomData(AtomId(42), Isotope(60), Charge(-9), True)
assert a == b
g = graphDFS("[60Mo9-.]")
v = next(iter(g.vertices))
print("Vertex:", v.atomId, v.isotope, v.charge, v.radical)
assert v.atomId == a.atomId
assert v.isotope == a.isotope
assert v.charge == a.charge
assert a.radical == a.radical

print("BondType\n----------")
for a in [BondType.Invalid, BondType.Single, BondType.Aromatic, BondType.Double, BondType.Triple]:
	print(repr(a))
	if a != BondType.Invalid:
		print(a)
print(BondType.values)

print("AtomId\n----------")
print(AtomIds.Invalid)
print(AtomIds.Max)
for k, v in AtomIds.__dict__.items():
	print(k, ":", v)
