graphGMLString("""
	graph [ node [ id 0 label "C" ] ]
""")
graphGML("graph.gml")
a = graphGML("graph.gml")
print("a:\t", a)
with open("myGraph.gml", "w") as f:
	f.write(a.getGMLString())
a = graphGML(CWDPath("myGraph.gml"))

graphDFS("OCC=O")
a = graphDFS("OCC=O")
print("a:\t", a)

smiles("O")
a = smiles("O")
print("a:\t", a)

smiles("[O]")
for a in inputGraphs: a.print()

print("inputGraphs:\t", inputGraphs)

print("id:\t", a.id)
print("str:\t", a)
print("repr:\t", repr(a))

a = graphDFS("COCC-O-C#N")
a.print()
a = graphDFS("COOC")
a.print()
a.printGML()
print(a.getGMLString())

print("name", "smiles", "graphDFS", "linearEncoding", sep="\t")
print(a.name, a.smiles, a.graphDFS, a.linearEncoding, sep="\t")

print("numVertices:\t", a.numVertices)
print("numEdges:\t", a.numEdges)
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

host = smiles("O=C=O", "CO2")
pattern = smiles("[C]=O", name="Pattern")
host2 = smiles("O=C=O", "CO2 2")

assert(host.isomorphism(host2) > 0)
assert(host.isomorphism(pattern) == 0)
assert(pattern.monomorphism(host) > 0)
assert(host.monomorphism(host2) > 0)
assert(host2.monomorphism(pattern) == 0)

a = smiles("O=C(O)C(CC(=O)O)C(O)C(=O)O", name="Isocitrate")
aPerm = a.makePermutation()

a = smiles("C*C")
a.print()
a = smiles("N[*]N")
a.print()


a = smiles("C")
a.print()
def customImage():
	with open("out/custom.tex", "w") as f:
		f.write("""\\begin{tikzpicture}
\\node {custom};
\\end{tikzpicture}""")
	return "out/custom"
a.image = customImage
a.imageCommand = "compileTikz \"out/custom\" \"out/custom\""
a.print()

# Check interface
a = graphDFS("COCC-O-C#N")
print("numVertices:", a.numVertices)
for v in a.vertices:
	print("Vertex:", v.graph, v.id, v.degree, v.stringLabel, v.atomId, v.isotope, v.charge, v.radical)
	for e in v.incidentEdges:
		print("\tEdge:", e.target.id)
print("numEdges:", a.numEdges)
for e in a.edges:
	print("Edge:", e.graph, e.source.id, e.target.id, e.stringLabel, e.bondType)

a = graphDFS("[A][B]")
v0 = a.vertices[0]
v1 = a.vertices[1]
assert v0 == v0
assert v0 != v1
assert v0 < v1
e1 = list(v0.incidentEdges)[0]
e2 = list(v1.incidentEdges)[0]
assert e1 == e2
assert not e1 != e2
assert e1 < e2











