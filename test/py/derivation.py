include("formoseCommon/grammar_H.py")

d = Derivation()
d.left = inputGraphs
d.rule = inputRules[0]
d.right = inputGraphs
print("Derivation:\t", d)
