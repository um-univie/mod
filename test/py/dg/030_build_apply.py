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

DG().build().apply([], r)
fail(lambda: DG().build().apply([None], r), "One of the graphs is a null pointer.")
fail(lambda: DG().build().apply([], None), "The rule is a null pointer.")
fail(lambda: DG(graphDatabase=[a]).build().apply([aa], r), "Isomorphic graphs. Candidate graph 'gaa' is isomorphic to 'ga' in the graph database.")
fail(lambda: DG(graphDatabase=[]).build().apply([a, aa], r), "Isomorphic graphs. Candidate graph 'gaa' is isomorphic to 'ga' in the graph database.")

dg = DG(graphDatabase=[a, b, c])
with dg.build() as builder:
	res = builder.apply([a], r)
	assert len(res) == 3
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b, c, b]

	config.dg.applyAssumeConfluence = True
	res = builder.apply([a], r)
	config.dg.applyAssumeConfluence = False
	assert len(res) == 1
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b]

	config.dg.applyLimit = 0
	res = builder.apply([a], r)
	assert len(res) == 0

	config.dg.applyLimit = 1
	res = builder.apply([a], r)
	assert len(res) == 1
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b]

	config.dg.applyLimit = 2
	res = builder.apply([a], r)
	assert len(res) == 2
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b, c]

	config.dg.applyLimit = 3
	res = builder.apply([a], r)
	assert len(res) == 3
	for e in res:
		assert list(e.rules) == [r]
		assert list(v.graph for v in e.sources) == [a]
		assert e.numTargets == 1
	ts = [next(iter(e.targets)).graph for e in res]
	assert ts == [b, c, b]

	config.dg.applyLimit = 4
	res = builder.apply([a], r)
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
		builder.apply([a], r, verbosity=verbosity)
