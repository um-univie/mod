include("xxx_helpers.py")

# ConstrainAdj
# ============================================================================
gmlFail('''constrainAdj [ id 0 op "=" count 0 ]''',
	"Vertex 0 in adjacency constraint does not exist.")
gmlFail('''context [ node [ id 0 label "A" ] ]
	constrainAdj [ id 0 op "a" count 0 ]''',
	"Unknown operator 'a' in adjacency constraint.")
