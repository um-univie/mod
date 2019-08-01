def check(s, withIds=False):
	if withIds:
		toString = lambda a: a.graphDFSWithIds
	else:
		toString = lambda a: a.graphDFS
	a = graphDFS(s)
	a.print()
	s1 = toString(a)
	print(s1)
	a1 = graphDFS(s1)
	s2 = toString(a1)
	a2 = graphDFS(s2)
	iso1 = a.isomorphism(a1)
	iso2 = a.isomorphism(a2)
	if iso1 == 0 or iso2 == 0:
		print("s: ", s)
		print("s1:", s1)
		print("s2:", s2)
		postSection("Error, iso1=%d, iso2=%d" % (iso1, iso2))
		a.print()
		a1.print()
		a2.print()
		assert False
	return a

postSection("Implicit vertices")
print("Implicit vertices")
for s in {"B", "C", "N", "O", "P", "S", "F", "Cl", "Br", "I"}:
	a = check(s)
	print(a.id, a.graphDFS)
	a.printGML()

postSection("Explicit implicit vertices")
print("Explicit implicit vertices")
for s in {"B", "C", "N", "O", "P", "S", "F", "Cl", "Br", "I"}:
	a = check("[" + s + "]")
	print(a.id, a.graphDFS)
	a.printGML()

postSection("Ids and Ring Closures")
print("Ids and Ring Closures")
check("O1CCC-1N")
check("O1CCC1N")

postSection("More Stuff")
print("More Stuff")
check("C1CCC1")
check("[C\]]")
check("C{a\}b}C")

postSection("All With Ids")
print("All With Ids")
check("NCC(O)C(O)=O", withIds=True)
