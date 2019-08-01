template = """rule [
    %s
    context [
        node [ id 0 label "H+" ]
    ]
]"""
lsString = LabelSettings(LabelType.String, LabelRelation.Unification)
lsTerm = LabelSettings(LabelType.Term, LabelRelation.Unification)
s = ruleGMLString(template % 'labelType "string"')
t = ruleGMLString(template % 'labelType "term"')
n = ruleGMLString(template % '')
for a in s, t, n:
    print("LabelType:", a.labelType)
    print("GML\n===========================")
    print(a.getGMLString())
    print("GML end\n===========================")

def fail(f, a, ls, o):
    try:
        f(a, ls, o)
        assert False
    except LogicError as e:
        print(e)
f = lambda a, ls, o: dgRuleComp([], a, ls, ignoreRuleLabelTypes=o)
f(s, lsString, False)
f(t, lsTerm, False)
fail(f, s, lsTerm, False)
fail(f, t, lsString, False)
f(s, lsTerm, True)
f(t, lsString, True)
