include("xxx_helpers.py")

# Vertex Properties
# ============================================================================
gmlFail('''
	left    [ node [ id 0           stereo "any" ] ]
	context [ node [ id 0 label "A" stereo "any" ] ]
''', "Vertex 0 has stereo both in 'context' and 'left'.")
gmlFail('''
	right   [ node [ id 0           stereo "any" ] ]
	context [ node [ id 0 label "A" stereo "any" ] ]
''', "Vertex 0 has stereo both in 'context' and 'right'.")

# check that context data is copied to left and right
gmlFail('context [ node [ id 0 label "A" stereo "blah" ] ]',
	"Error in stereo data for vertex 0 in L. Invalid geometry 'blah'.")


# Edge properties
# ============================================================================
gmlFail('''
	left [    edge [ source 0 target 1           stereo "" ] ]
	context [ edge [ source 0 target 1 label "-" stereo "" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has stereo both in 'context' and 'left'.")
gmlFail('''
	right [   edge [ source 0 target 1           stereo "" ] ]
	context [ edge [ source 0 target 1 label "-" stereo "" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has stereo both in 'context' and 'right'.")

# check that context data is copied to left and right
gmlFail('''context [
	node [ id 0 label "C" ]
	node [ id 1 label "C" ]
	edge [ source 0 target 1 label "-" stereo "blah" ]
]''', "Error in stereo data for edge (0, 1) in L. Parsing error in stereo data 'blah'.")


# Embedding
# ============================================================================

# parsing error
gmlFail('left [ node [ id 0 label "C" stereo "" ] ]',
	"Error in stereo data for vertex 0 in L. Parsing failed:")
gmlFail('left [ node [ id 0 label "C" stereo " " ] ]',
	"Error in stereo data for vertex 0 in L. Parsing failed:")
gmlFail('left [ node [ id 0 label "C" stereo "tetrahedral[" ] ]',
	"Error in stereo data for vertex 0 in L. Parsing failed:")
gmlFail('left [ node [ id 0 label "C" stereo "tetrahedral[1" ] ]',
	"Error in stereo data for vertex 0 in L. Parsing failed:")
gmlFail('left [ node [ id 0 label "C" stereo "tetrahedral[.]" ] ]',
	"Error in stereo data for vertex 0 in L. Parsing failed:")
# higher level parsing error
gmlFail('left [ node [ id 0 label "C" stereo "tetrahedral[a]" ] ]',
	"Error in graph GML. Virtual neighbour in stereo embedding for vertex 0 in L has unknown type 'a'.")

# not all edges, do offset to force the vertexPrinter to fail if wrong mapping
gmlFail('''right [
		node [ id 0 label "offset" ]
	]
	left [
		node [ id 10 label "C" stereo "tetrahedral[]" ]
		node [ id 11 label "H" ] edge [ source 10 target 11 label "-" ]
		node [ id 12 label "H" ] edge [ source 10 target 12 label "-" ]
		node [ id 13 label "H" ] edge [ source 10 target 13 label "-" ]
		node [ id 14 label "H" ] edge [ source 10 target 14 label "-" ]
]''', "Too few edges in stereo embedding for vertex 10 in L. Got 0 edges, but the degree is 4.")
gmlFail('''left [
		node [ id 0 label "offset" ]
	]
	right [
		node [ id 10 label "C" stereo "tetrahedral[]" ]
		node [ id 11 label "H" ] edge [ source 10 target 11 label "-" ]
		node [ id 12 label "H" ] edge [ source 10 target 12 label "-" ]
		node [ id 13 label "H" ] edge [ source 10 target 13 label "-" ]
		node [ id 14 label "H" ] edge [ source 10 target 14 label "-" ]
]''', "Too few edges in stereo embedding for vertex 10 in R. Got 0 edges, but the degree is 4.")

# duplicate edge
gmlFail('''left [
	node [ id 10 label "C" stereo "tetrahedral[11, 12, 13, 13]" ]
	node [ id 11 label "H" ] edge [ source 10 target 11 label "-" ]
	node [ id 12 label "H" ] edge [ source 10 target 12 label "-" ]
	node [ id 13 label "H" ] edge [ source 10 target 13 label "-" ]
	node [ id 14 label "H" ] edge [ source 10 target 14 label "-" ]
]''', "Duplicate edge in stereo embedding for vertex 10 in L.")

# duplicate radical
gmlFail('''left [
	node [ id 10 label "C" stereo "tetrahedral[11, 12, r, r]" ]
	node [ id 11 label "H" ] edge [ source 10 target 11 label "-" ]
	node [ id 12 label "H" ] edge [ source 10 target 12 label "-" ]
]''', "Multiple radicals in stereo embedding for vertex 10 in L.")
