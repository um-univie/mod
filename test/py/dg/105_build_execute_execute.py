include("1xx_execute_helpers.py")

exeStrat(DGStrat.makeExecute(False), [], [])
exeStrat(execute(False), [], [])

g1 = smiles("C", "g1")
g2 = smiles("N", "g2")

b = False
def f(gs):
	global b
	print("f has been called")
	b = True
exeStrat(addSubset(g1, g2) >> execute(f), [g1, g2], [g1, g2])
assert b
