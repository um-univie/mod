include("../xxx_helpers.py")
post.enableInvokeMake()
a = Rule.fromDFS("[O][C][C][C]1[C]2[C]3[C]4[C]5[C][S][P]>>[N][C][C][C]1[C]2[C]3[C]4[C]5[C][S][P]")
a = Rule.fromGMLString('rule [ context [ node [ id 0 label "C" ] ] ]')

post.summaryChapter("First")
p = GraphPrinter()
f11 = a.print(p)
p.withGraphvizCoords = True
f12 = a.print(p)
p.graphvizPrefix = 'layout = "dot"';
f13 = a.print(p)

post.summaryChapter("Second")
p = GraphPrinter()
f21 = a.print(p)
p.withGraphvizCoords = True
f22 = a.print(p)
p.graphvizPrefix = 'layout = "dot"';
f23 = a.print(p)
