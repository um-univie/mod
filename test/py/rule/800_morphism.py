include("xxx_helpers.py")

data = """rule [
	left  [
		node [ id 11 label "vL1" ]
		node [ id 12 label "vL2" ]
		edge [ source 11 target 12 label "eL" ]

		node [ id 41 label "vCL1" ]
		node [ id 42 label "vCL2" ]
		edge [ source 41 target 42 label "eCL" ]
	]
	context [
		node [ id 31 label "vK1" ]
		node [ id 32 label "vK2" ]
		edge [ source 31 target 32 label "eK" ]
	]
	right [
		node [ id 21 label "vR1" ]
		node [ id 22 label "vR2" ]
		edge [ source 21 target 22 label "eR" ]

		node [ id 41 label "vCR1" ]
		node [ id 42 label "vCR2" ]
		edge [ source 41 target 42 label "eCR" ]
	]
]"""
print("Loading r1")
print("=" * 80)
r1 = Rule.fromGMLString(data)
print("Loading r2")
print("=" * 80)
r2 = Rule.fromGMLString(data)
print("Isomorphism")
print("=" * 80)
assert r1.isomorphism(r2) != 0
print("Monoomorphism")
print("=" * 80)
assert r1.monomorphism(r2) != 0

commonChecks(r1)
commonChecks(r2)
