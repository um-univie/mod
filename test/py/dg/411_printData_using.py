include("../xxx_helpers.py")
post.enableInvokeMake()

dg = DG()
with dg.build() as b:
	b.addAbstract("""
		A -> B
		C + D -> E + F
	""")

	fail(lambda: DGPrintData(dg), "Can not create print data. The DG is not locked yet.")
e1 = next((e for e in dg.edges if e.numSources == 1))
e2 = next((e for e in dg.edges if e.numSources == 2))
for v in dg.vertices:
	globals()[v.graph.name] = v

post.summarySection("makeDuplicate")
d = DGPrintData(dg)
d.makeDuplicate(e1, 1)
dg.print(data=d)
p = DGPrinter()
p.withShortcutEdges = False
dg.print(data=d, printer=p)

post.summarySection("removeDuplicate")
d = DGPrintData(dg)
d.removeDuplicate(e1, 0)
dg.print(data=d)

post.summarySection("reconnectSource")
d = DGPrintData(dg)
d.makeDuplicate(e2, 1)
d.reconnectSource(e2, 1, C, 1)
dg.print(data=d)

post.summarySection("reconnectTarget")
d = DGPrintData(dg)
d.makeDuplicate(e2, 1)
d.reconnectTarget(e2, 1, E, 1)
dg.print(data=d)
