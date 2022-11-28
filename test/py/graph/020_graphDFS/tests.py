include("common.py")

def sec(s):
	post.summarySection(s)
	print("="*80)
	print(s)

sec("Implicit vertices")
for s in {"B", "C", "N", "O", "P", "S", "F", "Cl", "Br", "I"}:
	a = check(s)

sec("Explicit implicit vertices")
for s in {"B", "C", "N", "O", "P", "S", "F", "Cl", "Br", "I"}:
	a = check("[" + s + "]")

sec("Ids and Ring Closures")
check("O1CCC-1N")
check("O1CCC1N")
check("[A]1[B][C]-1([S])")
check("[A]1[B][C]1")

sec("Branches")
check("NCC(O)C(O)=O")

sec("More Stuff")
check("C1CCC1")
check(r"[C\]]")
check(r"C{a\}b}C")
