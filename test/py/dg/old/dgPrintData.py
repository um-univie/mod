include("../../formoseCommon/grammar.py")

dg = dgRuleComp(inputGraphs, addSubset(inputGraphs)
	>> rightPredicate[lambda d: all(a.vLabelCount("C") <= 4 for a in d.right)](
		repeat(inputRules)
	)
)
dg.calc()

data = DGPrintData(dg)
printer = DGPrinter()
postSection("Default")
dg.print(printer, data)
postSection("Individual Formaldehyde")
for e in dg.edges:
	if formaldehyde in (v.graph for v in e.sources):
		data.reconnectTail(e, 0, formaldehyde, e.id)
	if formaldehyde in (v.graph for v in e.targets):
		data.reconnectHead(e, 0, formaldehyde, e.id)
dg.print(printer, data)
postSection("All Duplicates")
data = DGPrintData(dg)
for e in dg.edges:
	for v in e.sources: data.reconnectTail(e, 0, v.graph, e.id)
	for v in e.targets: data.reconnectHead(e, 0, v.graph, e.id)
dg.print(printer, data)
