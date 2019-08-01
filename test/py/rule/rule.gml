rule [
	ruleID  "Test Rule"
	left [
		node [ id 1 label "L" ]
		node [ id 4 label "LC_A" ]
		edge [ source 1 target 4 label "-" ]
	]
	context [
		node [ id 3 label "C" ]
		edge [ source 3 target 4 label "-" ]
	]
	right [
		node [ id 2 label "R" ]
		node [ id 4 label "LC_B" ]
		edge [ source 2 target 4 label "-" ]
	]
	constrainAdj [
		id 1 count 0 op "="
		nodeLabels [ label "A" label "B" ]
		edgeLabels [ label "Q" label "R" ]
	]
	constrainAdj [
		id 4 count 2 op "<"
		nodeLabels [ label "C" label "D" ]
		edgeLabels [ label "S" label "T" ]
	]
]
