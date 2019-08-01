_ = ruleGMLString("""rule [ ruleID "->"
]""");
_A = ruleGMLString("""rule [ ruleID "-> A"
	right [
		node [ id 0 label "A" ]
	]
]""");
_AeA = ruleGMLString("""rule [ ruleID "-> A A"
	right [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
]""");
_AAA = ruleGMLString("""rule [ ruleID "-> AAA"
	right [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
_ABA = ruleGMLString("""rule [ ruleID "-> ABA"
	right [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
]""");
_AAB = ruleGMLString("""rule [ ruleID "-> AAB"
	right [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
_B = ruleGMLString("""rule [ ruleID "-> B"
	right [
		node [ id 0 label "B" ]
	]
]""");
A_ = ruleGMLString("""rule [ ruleID "A ->"
	left [
		node [ id 0 label "A" ]
	]
]""");
A__A = ruleGMLString("""rule [ ruleID "A -> other A"
	left [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 1 label "A" ]
	]
]""");
A_A = ruleGMLString("""rule [ ruleID "A -> A"
	context [
		node [ id 0 label "A" ]
	]
]""");
A_AeA = ruleGMLString("""rule [ ruleID "A -> A A"
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 1 label "A" ]
	]
]""");
A_AAA = ruleGMLString("""rule [ ruleID "A -> AAA"
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
A_ABA = ruleGMLString("""rule [ ruleID "A -> ABA"
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
]""");
A_AAB = ruleGMLString("""rule [ ruleID "A -> AAB"
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 1 label "B" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
A_B = ruleGMLString("""rule [ ruleID "A -> B"
	left [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 0 label "B" ]
	]
]""");
A_BAA = ruleGMLString("""rule [ ruleID "A -> BAA"
	left [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 0 label "B" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
A_C = ruleGMLString("""rule [ ruleID "A -> C"
	left [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 0 label "C" ]
	]
]""");
AeA_ = ruleGMLString("""rule [ ruleID "A A ->"
	left [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
]""");
AeA_A = ruleGMLString("""rule [ ruleID "A A -> A"
	left [
		node [ id 0 label "A" ]
	]
	context [
		node [ id 1 label "A" ]
	]
]""");
AeA_AeA = ruleGMLString("""rule [ ruleID "A A -> A A"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
]""");
AeA_AAA = ruleGMLString("""rule [ ruleID "A A -> AAA"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "A" ]
	]
]""");
AeA_ABA = ruleGMLString("""rule [ ruleID "A A -> ABA"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "B" ]
	]
]""");
AeB_AAB = ruleGMLString("""rule [ ruleID "AeB -> AAB"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
	right [
		edge [ source 0 target 1 label "A" ]
	]
]""");
AAA_ = ruleGMLString("""rule [ ruleID "AAA ->"
	left [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
AAA_A = ruleGMLString("""rule [ ruleID "AAA -> A"
	left [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
	]
]""");
AAA_AeA = ruleGMLString("""rule [ ruleID "AAA -> A A"
	left [
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
]""");
AAA_AAA = ruleGMLString("""rule [ ruleID "AAA -> AAA"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
AAA_ABA = ruleGMLString("""rule [ ruleID "AAA -> ABA"
	left [
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "B" ]
	]
]""");
AAA_ACA = ruleGMLString("""rule [ ruleID "AAA -> ACA"
	left [
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "C" ]
	]
]""");
AAB_ = ruleGMLString("""rule [ ruleID "AAB ->"
	left [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
AAB_AeB = ruleGMLString("""rule [ ruleID "AAB -> A B"
	left [
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
]""");
AAB_AAB = ruleGMLString("""rule [ ruleID "AAB -> AAB"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
AAB_ABB = ruleGMLString("""rule [ ruleID "AAB -> ABB"
	left [
		edge [ source 0 target 1 label "A" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "B" ]
	]
	right [
		edge [ source 0 target 1 label "B" ]
	]
]""");
ABA_ = ruleGMLString("""rule [ ruleID "ABA ->"
	left [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
]""");
ABA__AAA = ruleGMLString("""rule [ ruleID "ABA -> AAA other"
	left [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
	]
	right [
		node [ id 2 label "A" ]
		edge [ source 0 target 2 label "A" ]
	]
]""");
ABA_A = ruleGMLString("""rule [ ruleID "ABA -> A"
	left [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
	]
]""");
ABA_AeA = ruleGMLString("""rule [ ruleID "ABA -> A A"
	left [
		edge [ source 0 target 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
]""");
ABA_AAA = ruleGMLString("""rule [ ruleID "ABA -> AAA"
	left [
		edge [ source 0 target 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "A" ]
	]
]""");
ABA_ABA = ruleGMLString("""rule [ ruleID "ABA -> ABA"
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "B" ]
	]
]""");
ABA_ACA = ruleGMLString("""rule [ ruleID "ABA -> ACA"
	left [
		edge [ source 0 target 1 label "B" ]
	]
	context [
		node [ id 0 label "A" ]
		node [ id 1 label "A" ]
	]
	right [
		edge [ source 0 target 1 label "C" ]
	]
]""");

B_ = ruleGMLString("""rule [ ruleID "B ->"
	left [
		node [ id 0 label "B" ]
	]
]""");
B_A = ruleGMLString("""rule [ ruleID "B -> A"
	left [
		node [ id 0 label "B" ]
	]
	right [
		node [ id 0 label "A" ]
	]
]""");
B_B = ruleGMLString("""rule [ ruleID "B -> B"
	context [
		node [ id 0 label "B" ]
	]
]""");
B_C = ruleGMLString("""rule [ ruleID "B -> C"
	left [
		node [ id 0 label "B" ]
	]
	right [
		node [ id 0 label "C" ]
	]
]""");
B_BAA = ruleGMLString("""rule [ ruleID "B -> BAA"
	context [
		node [ id 0 label "B" ]
	]
	right [
		node [ id 1 label "A" ]
		edge [ source 0 target 1 label "A" ]
	]
]""");
