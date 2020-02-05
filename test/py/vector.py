a = smiles("O")

assert VecGraph() == VecGraph()
assert VecGraph() == []
assert [] == VecGraph()
l = VecGraph()
l.append(a)
assert l == [a]
assert [a] == l
