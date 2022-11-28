include("../../xxx_helpers.py")

def check(s):
	def iso(g1, g2):
		if g1.isomorphism(g2) == 0:
			print("      Error")
			g1.print()
			g2.print()
			post.enableInvokeMake()
			assert False
		

	print("check({})".format(s))
	a = Graph.fromDFS(s)

	s1 = a.graphDFS
	print("  min. ids:", s1)
	a1 = Graph.fromDFS(s1)
	s2 = a1.graphDFS
	s2id = a1.graphDFSWithIds
	print("    min. ids:", s2)
	print("    all ids: ", s2id)
	iso(a, a1)


	s1 = a.graphDFSWithIds
	print("  all ids: ", s1)
	a1 = Graph.fromDFS(s1)
	s2 = a1.graphDFS
	s2id = a1.graphDFSWithIds
	print("    min. ids:", s2)
	print("    all ids: ", s2id)
	iso(a, a1)

	return a

def checkMulti(s):
	print("checkMulti({})".format(s))
	gs = Graph.fromDFSMulti(s)
