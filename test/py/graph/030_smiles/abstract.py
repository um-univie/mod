include("common.py")

def check(p, name):
	a = smiles("[{}]".format(p.format(name)), allowAbstract=True)
	s = next(iter(a.vertices)).stringLabel
	if s != name:
		print("name:  ", name)
		print("actual:", s)
		assert False
	return a

check("{}", "C")
a = check("{}:42", "C")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = True
a = check("{}", "C:42")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = False

check("{}", "*")
a = check("{}:42", "*")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = True
a = check("{}:42", "*")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = False

check("{}", "abc")
a = check("{}:42", "abc")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = True
a = check("{}", "abc:42")
assert a.getVertexFromExternalId(42)
config.graph.appendSmilesClass = False

check("{}", "abc")
check("{}", "[]")
check("{}", "[def]")
check("{}", "abc[def]")
check("{}", "[def]efg")
check("{}", "abc[def]efg")
check("{}", "abc[def]ghi[jkl]mno")
check("{}", "[abc][def][ghi]")

check("{}", "42")
check("{}", "42Heblah")
check("{}", "He@blah")
check("{}", "HeHblah")
check("{}", "He+blah")
check("{}", "He.blah")
