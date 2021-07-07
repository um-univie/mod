include("common.py")

smiles("[H][H]")
smiles("CNOPS")
for a in inputGraphs:
	print("-" * 60)
	print("SMILES:", a.smiles)
	print("WithId:", a.smilesWithIds)
