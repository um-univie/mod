include("../xxx_helpers.py")

data = """rule [
left  [ node [ id 0 label "L" ] ]
right [ node [ id 0 label "R" ] ]
]"""
fGML = 'myRule.gml'
with open("myRule.gml", "w") as f:
	f.write(data)

inputRules[:] = []
r1 = Rule.fromGMLString(data)
assert inputRules == [r1]
r2 = Rule.fromGMLString(data)
assert inputRules == [r1, r2]
Rule.fromGMLString(data, add=False)
assert inputRules == [r1, r2]

assert Rule.fromGMLString == ruleGMLString

inputRules[:] = []
r1 = Rule.fromGMLFile(CWDPath(fGML))
assert inputRules == [r1]
r2 = Rule.fromGMLFile(CWDPath(fGML))
assert inputRules == [r1, r2]
Rule.fromGMLFile(CWDPath(fGML), add=False)
assert inputRules == [r1, r2]
assert Rule.fromGMLFile == ruleGML

fail(lambda: Rule.fromGMLFile("doesNotExist.gml"),
	"Could not open rule GML file ", err=InputError, isSubstring=True)
