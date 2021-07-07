a = ruleGMLString('rule [ context [ node [ id 0 label "C" ] ] ]')
fs = a.print()
assert len(fs) == 2
assert fs[0] != fs[1]

p = GraphPrinter()
fs = a.print(first=p, second=p)
assert len(fs) == 2
assert fs[0] == fs[1]

p2 = GraphPrinter()
fs = a.print(first=p, second=p2)
assert len(fs) == 2
assert fs[0] == fs[1]


a.print(printCombined=True)
