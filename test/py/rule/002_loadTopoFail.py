include("xxx_helpers.py")

for side in "left", "context", "right":
	ruleFail(''' %s [
		node [ id 0 label "A" ]
		edge [ source 0 target 1 label "-" ]
	]''' % side, "Edge endpoint '1' does not exist for edge (0, 1).")
	ruleFail('''%s [
		node [ id 0 label "A" ]
		edge [ source 1 target 0 label "-" ]
	]''' % side, "Edge endpoint '1' does not exist for edge (0, 1).")

for side in "left", "context", "right":
	ruleFail(""" %s [
		node [ id 0 label "C" ]
		edge [ source 0 target 0 label "-" ]
	]""" % side, "Loop edge (on 0, in")

	ruleFail(""" %s [
		node [ id 0 label "C" ]
		node [ id 1 label "C" ]
		edge [ source 0 target 1 label "-" ]
		edge [ source 1 target 0 label "-" ]
	] """ % side, "Duplicate edge (1, 0) in ")

	ruleFail(""" %s [
		node [ id 0 label "C" ]
		node [ id 0 label "C" ]
	] """ % side, "Duplicate vertex 0 in ")
