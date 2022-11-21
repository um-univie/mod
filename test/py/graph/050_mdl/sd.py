post.disableInvokeMake()

mol = """
 OpenBabel03141812452D

  3  2  0  0  0  0  0  0  0  0999 V2000
    0.0000    0.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0
    0.0000    0.0000    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0
  1  2  1  0  0  0  0
  2  3  1  0  0  0  0
M  END
"""

def fail(s):
	try:
		res = Graph.fromSDString(s)
		assert False, res
	except InputError as e:
		print(e)

fail(mol)
Graph.fromSDString(mol + "$$$$")
Graph.fromSDString(mol + "$$$$\n")
fail(mol + "\n")
fail(mol + "a")
fail(mol + ">")
fail(mol + ">\na")
fail(mol + ">\n>")
fail(mol + ">\n>\n\n")
fail(mol + ">\n>\n\na")
Graph.fromSDString(mol + ">\n>\n\n$$$$")
Graph.fromSDString(mol + ">\n>\n\n$$$$\n")
fail(mol + ">\n>\n\n$$$$\na")
