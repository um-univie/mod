include("xxx_helpers.py")

r1 = Rule.fromDFS("[A] 1 ( [B] ) [C] [D] {E} [F] . [G] >> [H]")
r2 = Rule.fromDFS("[A]1([B])[C][D]{E}[F].[G]>>[H]")

def cmpGraphs(g1, g2):
	assert g1.numVertices == g2.numVertices
	assert g1.numEdges == g2.numEdges
	assert len(list(g1.vertices)) == len(list(g2.vertices))
	assert len(list(g1.edges)) == len(list(g2.edges))
	for v1, v2 in zip(g1.vertices, g2.vertices):
		assert v1.id == v2.id
		assert v1.degree == v2.degree
		assert len(list(v1.incidentEdges)) == len(list(v2.incidentEdges))
		for e1, e2 in zip(v1.incidentEdges, v2.incidentEdges):
			assert e1.source.id == e2.source.id
			assert e1.target.id == e2.target.id
	for e1, e2 in zip(g1.edges, g2.edges):
		assert e1.source.id == e2.source.id
		assert e1.target.id == e2.target.id

def cmpGraphsLab(g1, g2):
	cmpGraphs(g1, g2)
	for v1, v2 in zip(g1.vertices, g2.vertices):
		assert v1.stringLabel == v2.stringLabel
		for e1, e2 in zip(v1.incidentEdges, v2.incidentEdges):
			assert e1.stringLabel == e2.stringLabel
	for e1, e2 in zip(g1.edges, g2.edges):
		assert e1.stringLabel == e2.stringLabel

cmpGraphs(r1, r2)
cmpGraphsLab(r1.left, r2.left)
cmpGraphs(r1.context, r2.context)
cmpGraphsLab(r1.right, r2.right)

commonChecks(r1)
commonChecks(r2)
