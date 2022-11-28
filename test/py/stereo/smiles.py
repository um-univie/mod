post.summarySection("prev, branch, branch, tail")
smiles("O[C@](N)(P)S").print()
smiles("O[C@@](N)(P)S").print()
post.summarySection("prev, branch, branch, branch")
smiles("O[C@](N)(P)(S)").print()
smiles("O[C@@](N)(P)(S)").print()
post.summarySection("branch, branch, branch, tail")
smiles("[C@](O)(N)(P)S").print()
smiles("[C@@](O)(N)(P)S").print()

post.summarySection("prev, H, branch, tail")
smiles("O[C@H](P)S").print()
smiles("O[C@@H](P)S").print()
post.summarySection("prev, H, branch, branch")
smiles("O[C@H](P)(S)").print()
smiles("O[C@@H](P)(S)").print()
post.summarySection("branch, H, branch, tail")
smiles("[C@H](N)(P)S").print()
smiles("[C@@H](N)(P)S").print()

post.summarySection("ring closure")
inputGraphs[:] = []
smiles("C1O[C@](N)(P)S1", name="1").print()
smiles("O1[C@](N)(P)SC1", name="2").print()
smiles("[C@]1(N)(P)SCO1", name="3").print()
smiles("N[C@]1(SCO1)(P)", name="4").print()
smiles("O1CS[C@]1(P)(N)", name="5").print()
for a in inputGraphs:
	for b in inputGraphs:
		res = a.isomorphism(b, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism, LabelRelation.Isomorphism))
		if not res:
			print(a.name, "not isomorphic to", b.name)
			print("{}: {}".format(a.name, a.smiles))
			print("{}: {}".format(b.name, b.smiles))
			assert False

post.summarySection("from OpenSMILES")
smiles("FC1C[C@](Br)(Cl)CCC1").print()
smiles("[C@]1(Br)(Cl)CCCC(F)C1").print()
