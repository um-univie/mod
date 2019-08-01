def doStuff(s):
	dg = dgAbstract(s)
	dg.print()
	print("-"*80)
	for e in dg.edges:
		print(e)

doStuff("a -> b")
doStuff("a <=> b")
doStuff("a + b -> a")
doStuff("2 a + b -> 3 c")
doStuff("a -> b b -> d + a d -> c c -> b")
# test  null reactions
doStuff("a -> b b + c -> b + c c + d <=> c + d d -> e")
# test coefficient stuff
doStuff("2a -> 2 b")
