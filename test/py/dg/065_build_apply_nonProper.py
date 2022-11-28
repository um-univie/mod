include("xx0_helpers.py")

a = smiles("[C][C][C]", name="ga")
b = smiles("[O][C][C]")
c = smiles("[C][O][C]")
aa = smiles("[C][C][C]", name="gaa")

r = ruleGMLString("""rule [
	left [
		node [ id 0 label "C" ]
	]
	right [
		node [ id 0 label "O" ]
	]
]""")

DG().build().apply([], r, onlyProper=False)
DG().build().apply([a], ruleGMLString("""rule [
	right [
		node [ id 0 label "O" ]
	]
]"""), onlyProper=False)

fail(lambda: DG().build().apply([None], r, onlyProper=False), "One of the graphs is a null pointer.")
fail(lambda: DG().build().apply([], None, onlyProper=False), "The rule is a null pointer.")
fail(lambda: DG(graphDatabase=[a]).build().apply([aa], r, onlyProper=False), "Isomorphic graphs. Candidate graph 'gaa' is isomorphic to 'ga' in the graph database.")
fail(lambda: DG(graphDatabase=[]).build().apply([a, aa], r, onlyProper=False), "Isomorphic graphs. Candidate graph 'gaa' is isomorphic to 'ga' in the graph database.")

dg = DG(graphDatabase=[a, b, c])
with dg.build() as builder:
	print("Apply")
	print("=" * 60)
	res = builder.apply([a], r, onlyProper=False)
	assert len(res) == 3
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b, c, b]

	for verbosity in (2, 10, 20):
		print("Verbosity:", verbosity)
		print("#" * 80)
		builder.apply([a], r, verbosity=verbosity, onlyProper=False)


print("Multiple components, different")
print("=" * 60)

gO = smiles('[O]', "gO")
gC = smiles('[C]', "gC")
gOC = smiles('[O][C]', "gOC")

r = ruleGMLString("""rule [
	ruleID "Connect O C"
	context [
		node [ id 0 label "O" ]
		node [ id 1 label "C" ]
	]
	right [
		edge [ source 0 target 1 label "-" ]
	]
]""")

dg = DG(graphDatabase=[gO, gC, gOC])
with dg.build() as builder:
	res = builder.apply([gO, gC], r, verbosity=4, onlyProper=False)
	assert len(res) == 1
	for e in res:
		assert list(e.rules) == [r]
		assert set(v.graph for v in e.sources) == set([gO, gC])
		assert e.numTargets == 1
		ts = [next(iter(e.targets)).graph for e in res]
		assert ts == [gOC]

	res = builder.apply([gO, gC, gC], r, verbosity=4, onlyProper=False)
	assert len(res) == 2
	for e in res:
		assert list(e.rules) == [r]
		assert set(v.graph for v in e.sources) == set([gO, gC])
		assert e.numTargets == 1
		ts = [next(iter(e.targets)).graph]
		assert ts == [gOC]


print("Multiple components, same")
print("=" * 60)

gO = smiles('[O]', "gO")
gOO = smiles('[O][O]', "gOO")

r = ruleGMLString("""rule [
	ruleID "Connect O O"
	context [
		node [ id 0 label "O" ]
		node [ id 1 label "O" ]
	]
	right [
		edge [ source 0 target 1 label "-" ]
	]
]""")

dg = DG(graphDatabase=[gO, gOO])
with dg.build() as builder:
	res = builder.apply([gO, gO], r, verbosity=4, onlyProper=False)
	assert len(res) == 1
	for e in res:
		assert list(e.rules) == [r]
		assert set(v.graph for v in e.sources) == set([gO, gO])
		assert e.numTargets == 1
		ts = [next(iter(e.targets)).graph]
		assert ts == [gOO]

	res = builder.apply([gO], r, verbosity=4, onlyProper=False)
	assert len(res) == 0


print("Split to isomorphic")
print("=" * 60)

r = ruleGMLString("""rule [
	ruleID "Split to isomorphms"
	left [
		edge [ source 1 target 2 label "-" ]
	]
	context [
		node [ id 1 label "Q" ]
		node [ id 2 label "Q" ]
	]
]""")
g1 = graphDFS("[Q][Q]", "QQ")
g2 = graphDFS("[Q]", "Q")

dg = DG(graphDatabase=[g1, g2])
with dg.build() as builder:
	res = builder.apply([g1], r, verbosity=4, onlyProper=False)
	assert len(res) == 2
	for e in res:
		assert list(e.rules) == [r]
		assert set(v.graph for v in e.sources) == set([g1])
		assert e.numTargets == 2
		ts = [v.graph for v in e.targets]
		assert ts == [g2, g2]


print("Empty result")
gC = smiles('[C]', "gC")
rRemove = ruleGMLString('rule [ left [ node [ id 0 label "C" ] ] ]')
dg = DG()
with dg.build() as builder:
	res = builder.apply([gC], rRemove, onlyProper=False, verbosity=4)
	assert len(res) == 0
