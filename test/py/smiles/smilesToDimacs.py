old_smiles = smiles
graphs = []
def smiles(s):
	g = old_smiles(s)
	if any(g.isomorphism(a) for a in graphs):
		return
	graphs.append(g)

config.graph.ignoreStereoInSmiles = True
config.graph.printSmilesParsingWarnings = False
include("smiles_cansmi_roundtrip.py")
include("smiles_nci.py")
include("problematic.py")
include("rhea.py")

graphs = sorted(graphs, key=lambda a: (a.numVertices, a.numEdges))
i = 1
for g in graphs:
	with open("out/mol_%04d.dimacs" % i, 'w') as f:
		f.write("p edge %d %d\n" % (g.numVertices, g.numEdges))
		for v in g.vertices:
			f.write("n %d %d\n" % (v.id + 1, v.atomId))
		for e in g.edges:
			f.write("e %d %d\n" % (e.source.id + 1, e.target.id + 1))
	i += 1


