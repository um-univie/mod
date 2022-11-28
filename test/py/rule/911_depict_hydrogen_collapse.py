def p(dfs):
	print(dfs)
	post.summarySection(dfs)
	r = Rule.fromDFS(dfs)
	pr = GraphPrinter()
	r.print(pr)
	pr.setReactionDefault()
	r.print(pr)

p("[C]1[H]2>>[N]1[O]2")
p("[N]1[O]2>>[C]1[H]2")

p("[C]1[H]2>>[C]1[H]2")

p("[ox]{oq}[oy].[C]1[H]2>>[C]1[H]2")
p("[C]1[H]2>>[ox]{oq}[oy].[C]1[H]2")
