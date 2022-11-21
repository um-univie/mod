ls = LabelSettings(LabelType.Term, LabelRelation.Unification)
rId = ruleGMLString("""rule [
    context [
        node [ id 0 label "C" ]
    ]
    right [
        node [ id 1 label "R" ]
        edge [ source 0 target 1 label "-" ]
    ]
]""")
rIdInv = rId.makeInverse()
rStr = """rule [
    context [
        node [ id 0 label "C" ]
        node [ id 1 label "Q" ]
        edge [ source 0 target 1 label "-" ]
    ]
    constrainAdj [
        id 1 op "=" count 1
        nodeLabels [ label "a" label "_x" label "b(c)" label "d(_y)" ]
        edgeLabels [ label "g" label "_p" label "h(i)" label "j(_q)" ]
    ]
]"""
a = ruleGMLString(rStr)
post.summaryChapter("TermState")
a.printTermState()
#b = a.makeInverse()

post.summaryChapter("Compose first")
rc = rcEvaluator([], ls)
res = rc.eval(a *rcSuper* rId)
for b in res:
    b.print()
    b.printTermState()

post.summaryChapter("Compose second")
rc = rcEvaluator([], ls)
res = rc.eval(rIdInv *rcSub* a)
for b in res:
    b.print()
    b.printTermState()

post.summaryChapter("DGRuleComp")
graphDFS("C[Q]")
dg = dgRuleComp([], addSubset(inputGraphs) >> a, ls)
dg.calc()
dg.print()
