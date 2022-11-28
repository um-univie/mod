post.disableInvokeMake()

lString = LabelSettings(LabelType.String, LabelRelation.Unification)
lTerm = LabelSettings(LabelType.Term, LabelRelation.Unification)

graphGMLString("""graph [
	node [ id 0 label "C" ]
	edge [ source 0 target 1 label "a" ]
	node [ id 1 label "A" ]
	edge [ source 0 target 2 label "b" ]
	node [ id 2 label "B" ]
]""")
ruleTemplate = """rule [
	ruleID "%s"
	left [
		node [ id 0 label "C" ]
	]
	right [
		node [ id 0 label "C(%s)" ]
	]
	constrainAdj [
		id 0 op "%s"
		count %d
		%s
	]
]"""
ops = {'lt': '<', 'leq': '<=', 'eq': '=', 'geq': '>=', 'gt': '>'}
def evalOp(a, op, b):
	if op == '<': return a < b
	if op == '<=': return a <= b
	if op == '=': return a == b
	if op == '>=': return a >= b
	if op == '>': return a > b
	assert False
nodeLabels = {
	'': '',
	'A': 'nodeLabels [ label "A" ]',
	'AB': 'nodeLabels [ label "A" label "B" ]',
}
edgeLabels = {
	'': '',
	'a': 'edgeLabels [ label "a" ]',
	'ab': 'edgeLabels [ label "a" label "b" ]',
}
trueCounts = {
	('',   ''): 2, ('',   'a'): 1, ('',   'ab'): 2,
	('A',  ''): 1, ('A',  'a'): 1, ('A',  'ab'): 1,
	('AB', ''): 2, ('AB', 'a'): 1, ('AB', 'ab'): 2
}
valid = set()
for count in range(0, 4):
	for opName, op in ops.items():
		for nlName, nl in nodeLabels.items():
			for elName, el in edgeLabels.items():
				if evalOp(trueCounts[(nlName, elName)], op, count):
					valid.add((nlName, elName, opName, count))
				name =  ','.join(a for a in [opName, str(count), nlName, elName] if len(a) > 0)
				labels = nl + "\n" + el + "\n"
				ruleGMLString(ruleTemplate % (name, name, op, count, labels))
def doDG(name, lSettings):
	print(name + "\n" + "="*50)
	dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> inputRules, labelSettings=lSettings)
	dg.calc()
	found = set()
	for vDG in dg.vertices:
		g = vDG.graph
		v = next(a for a in g.vertices if a.stringLabel.startswith("C"))
		l = v.stringLabel
		if l == "C": continue
		l = l[2:-1]
		if l.find(', ') != -1:
			ls = l.split(', ')
		else:
			ls = l.split(',')
		if len(ls[2:]) == 0:
			nl = ''
			el = ''
		elif len(ls[2:]) == 1:
			if ls[2] in nodeLabels:
				nl = ls[2]
				el = ''
			else:
				assert ls[2] in edgeLabels
				nl = ''
				el = ls[2]
		else:
			assert len(ls[2:]) == 2
			nl = ls[2]
			el = ls[3]
		t = (nl, el, ls[0], int(ls[1]))
		found.add(t)
		print(ls[2:], ops[ls[0]], ls[1])
	err = False
	for f in found:
		if f not in valid:
			print("Too much:", f)
			err = True
	for v in valid:
		if v not in found:
			print("Too little:", v)
			err = True
	if err:
		print(sorted(found))
		print(sorted(valid))
		assert False
doDG("String", lString)
doDG("Term", lTerm)
