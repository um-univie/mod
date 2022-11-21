include("xxx_helpers.py")

for side in "left", "context", "right":
	gmlFail(''' %s [
		node [ id 0 label "A" ]
		edge [ source 0 target 1 label "-" ]
	]''' % side, "Edge endpoint '1' does not exist for edge (0, 1).")
	gmlFail('''%s [
		node [ id 0 label "A" ]
		edge [ source 1 target 0 label "-" ]
	]''' % side, "Edge endpoint '1' does not exist for edge (0, 1).")

for side in "left", "context", "right":
	gmlFail(""" %s [
		node [ id 0 label "C" ]
		edge [ source 0 target 0 label "-" ]
	]""" % side, "Loop edge (on 0, in")

	gmlFail(""" %s [
		node [ id 0 label "C" ]
		node [ id 1 label "C" ]
		edge [ source 0 target 1 label "-" ]
		edge [ source 1 target 0 label "-" ]
	] """ % side, "Duplicate edge (1, 0) in ")

	gmlFail(""" %s [
		node [ id 0 label "C" ]
		node [ id 0 label "C" ]
	] """ % side, "Duplicate vertex 0 in ")

msg = "Edge (0, 1) dangling: edge is present in {} but endpoint {} only present in {}."

gmlFail("""
left [    edge [ source 0 target 1 label "-" ] ]
context [ node [ id 0 label "A" ] ]
right [   node [ id 1 label "B" ] ]
""", msg.format("L", 1, "R"))
gmlFail("""
left [    edge [ source 0 target 1 label "-" ] ]
context [ node [ id 1 label "A" ] ]
right [   node [ id 0 label "B" ] ]
""", msg.format("L", 0, "R"))

gmlFail("""
left [    node [ id 1 label "B" ] ]
context [ node [ id 0 label "A" ] ]
right [   edge [ source 0 target 1 label "-" ] ]
""", msg.format("R", 1, "L"))
gmlFail("""
left [    node [ id 0 label "B" ] ]
context [ node [ id 1 label "A" ] ]
right [   edge [ source 0 target 1 label "-" ] ]
""", msg.format("R", 0, "L"))
