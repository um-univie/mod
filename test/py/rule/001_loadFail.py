include("xxx_helpers.py")

# for Boost < 1.77 it's column 12, for 1.77 it's 13
ruleFail('blah', "Parsing failed at 1:")

ruleFail('labelType "blah"', "Unknown labelType 'blah'.")

# Vertex properties
# ============================================================================
ruleFail('''
	left    [ node [ id 0 label "A" ] ]
	context [ node [ id 0 label "A" ] ]
''', "Vertex 0 has a label both in 'context' and 'left'.")
ruleFail('''
	right   [ node [ id 0 label "A" ] ]
	context [ node [ id 0 label "A" ] ]
''', "Vertex 0 has a label both in 'context' and 'right'.")

ruleFail('''
	left    [ node [ id 0           stereo "any" ] ]
	context [ node [ id 0 label "A" stereo "any" ] ]
''', "Vertex 0 has stereo both in 'context' and 'left'.")
ruleFail('''
	right   [ node [ id 0           stereo "any" ] ]
	context [ node [ id 0 label "A" stereo "any" ] ]
''', "Vertex 0 has stereo both in 'context' and 'right'.")

ruleFail('left  [ node [ id 0 ] ]', "Vertex 0 is in L, but has no label.")
ruleFail('right [ node [ id 0 ] ]', "Vertex 0 is in R, but has no label.")

# Edge properties
# ============================================================================
ruleFail('''
	left [    edge [ source 0 target 1 label "-" ] ]
	context [ edge [ source 0 target 1 label "-" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has a label both in 'context' and 'left'.")
ruleFail('''
	right [   edge [ source 0 target 1 label "-" ] ]
	context [ edge [ source 0 target 1 label "-" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has a label both in 'context' and 'right'.")

ruleFail('''
	left [    edge [ source 0 target 1           stereo "" ] ]
	context [ edge [ source 0 target 1 label "-" stereo "" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has stereo both in 'context' and 'left'.")
ruleFail('''
	right [   edge [ source 0 target 1           stereo "" ] ]
	context [ edge [ source 0 target 1 label "-" stereo "" ]
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]''', "Edge (0, 1) has stereo both in 'context' and 'right'.")

ruleFail('''
	left [ edge [ source 0 target 1 ] ]
	context [ node [ id 0 label "A" ] node [ id 1 label "B" ] ]''',
	"Edge (0, 1) is in L, but has no label.")
ruleFail('''
	right [ edge [ source 0 target 1 ] ]
	context [ node [ id 0 label "A" ] node [ id 1 label "B" ] ]''',
	"Edge (0, 1) is in R, but has no label.")
