include("1xx_execute_helpers.py")

gO = smiles('[O]', "gO")
gC = smiles('[C]', "gC")
gN = smiles('[N]', "gN")
gOC = smiles('[O][C]', "gOC")

rConvertOC = ruleGMLString("""rule [
	ruleID "Convert O -> C"
	left  [ node [ id 0 label "O" ] ]
	right [ node [ id 0 label "C" ] ]
]""")

rConnectOC = ruleGMLString("""rule [
	ruleID "Connect O C"
	context [
		node [ id 0 label "O" ]
		node [ id 1 label "C" ]
	]
	right [
		edge [ source 0 target 1 label "-" ]
	]
]""")

print("Subset, not universe")
print("="*80)
exeStrat(addSubset(gO) >> rConvertOC, [gC], [gO, gC],
	graphDatabase=inputGraphs, verbosity=10)
exeStrat(addUniverse(gO) >> rConvertOC, [], [gO],
	graphDatabase=inputGraphs, verbosity=10)

print("Multiple, subset")
print("="*80)
exeStrat(addSubset(gO, gC) >> rConnectOC, [gOC], [gO, gC, gOC],
	graphDatabase=inputGraphs, verbosity=10)

print("Multiple, 1 subset, 1 universe")
print("="*80)
exeStrat(addSubset(gO) >> addUniverse(gC) >> rConnectOC, [gOC], [gO, gC, gOC],
	graphDatabase=inputGraphs, verbosity=10)

print("Empty result")
rRemove = ruleGMLString('rule [ left [ node [ id 0 label "C" ] ] ]')
exeStrat(addSubset(gC) >> rRemove, [], [gC], verbosity=10)
