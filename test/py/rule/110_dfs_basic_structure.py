include("xxx_helpers.py")

dfsCheck(">>", '')

dfsCheck("[A]>>", 'left [ node [ id 1 label "A" ] ]')
dfsCheck(">>[A]", 'right [ node [ id 1 label "A" ] ]')

dfsCheck("[A]1>>", 'left [ node [ id 1 label "A" ] ]')
dfsCheck(">>[A]1", 'right [ node [ id 1 label "A" ] ]')

dfsCheck("[A].[B]>>", '''left [
	node [ id 1 label "A" ]
	node [ id 2 label "B" ]
]''')
dfsCheck(">>[A].[B]", '''right [
	node [ id 1 label "A" ]
	node [ id 2 label "B" ]
]''')

dfsCheck("[A][B]>>", '''left [
	node [ id 1 label "A" ]
	node [ id 2 label "B" ]
	edge [ source 1 target 2 label "-" ]
]''')
dfsCheck(">>[A][B]", '''right [
	node [ id 1 label "A" ]
	node [ id 2 label "B" ]
	edge [ source 1 target 2 label "-" ]
]''')

dfsCheck("[A]1>>[A]1", 'context [ node [ id 1 label "A" ] ]')
dfsCheck("[A]1>>[C]1", '''
	left [ node [ id 1 label "A" ] ]
	right [ node [ id 1 label "C" ] ]
''')

dfsCheck("[A]1.[B]>>[C]1", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "B" ]
	]
	right [ node [ id 1 label "C" ] ]
''')
dfsCheck("[A]1>>[C]1.[D]", '''
	left [ node [ id 1 label "A" ] ]
	right [
		node [ id 1 label "C" ]
		node [ id 2 label "D" ]
	]
''')

dfsCheck("[A]1[B]>>[C]1", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "B" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [ node [ id 1 label "C" ] ]
''')
dfsCheck("[A]1>>[C]1[D]", '''
	left [ node [ id 1 label "A" ] ]
	right [
		node [ id 1 label "C" ]
		node [ id 2 label "D" ]
		edge [ source 1 target 2 label "-" ]
	]
''')

dfsCheck("[A]1[B]>>[C]1[D]", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "B" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 1 label "C" ]
		node [ id 3 label "D" ]
		edge [ source 1 target 3 label "-" ]
	]
''')

dfsCheck("[A]1[B]2>>[C]1[D]2", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "B" ]
	]
	context [
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 1 label "C" ]
		node [ id 2 label "D" ]
	]
''')

dfsCheck("[A]1{-}[B]2>>[C]1{=}[D]2", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "B" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 1 label "C" ]
		node [ id 2 label "D" ]
		edge [ source 1 target 2 label "=" ]
	]
''')

# other checks
dfsCheck("[A]1[C]>>[B][C]1", '''
	left [
		node [ id 1 label "A" ]
		node [ id 2 label "C" ]
		edge [ source 1 target 2 label "-" ]
	]
	right [
		node [ id 3 label "B" ]
		node [ id 1 label "C" ]
		edge [ source 3 target 1 label "-" ]
	]
''')
