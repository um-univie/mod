
smiles("[O]")
for a in inputGraphs: a.print()

print("inputGraphs:\t", inputGraphs)

a = graphDFS("COCC-O-C#N")
a.print()
a = graphDFS("COOC")
a.print()
a.printGML()
print(a.getGMLString())

print("name", "smiles", "graphDFS", "linearEncoding", sep="\t")
print(a.name, a.smiles, a.graphDFS, a.linearEncoding, sep="\t")

print("isMolecule:\t", a.isMolecule)
#print("energy:\t", a.energy)
a = graphDFS("COCC=O")
a.cacheEnergy(42)
print("energy:\t", a.energy)
print("exactMass:\t", a.exactMass)

print("vLabelCount:\t", a.vLabelCount("O"))
print("eLabelCount:\t", a.eLabelCount("="))

a.name = "Test"
print("setName:\t", a)
print("inputGraphs:\t", inputGraphs)

# Check eq operator
inputGraphs[:] = []

a = smiles("C")
print("Energy:", a.energy)

a = smiles("O=C(O)C(CC(=O)O)C(O)C(=O)O", name="Isocitrate")
aPerm = a.makePermutation()

a = smiles("C*C")
a.print()
a = smiles("N[*]N")
a.print()
