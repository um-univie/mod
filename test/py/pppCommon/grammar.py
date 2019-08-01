AcP = smiles("OP(O)(=O)OC(=O)C", "AcP") # Acetyl-Phosphate
G3P = smiles("C(C(C=O)O)OP(=O)(O)O", "G3P") # Glyceraldehyde 3-phosphate
E4P = smiles("OP(O)(=O)OCC(O)C(O)C=O", "E4P") # Erythrose-4-phosphate
R5P = smiles("OP(O)(=O)OCC(O)C(O)C(O)C=O", "R5P") # Ribose 5-phosphate
Ru5P = smiles("OCC(=O)C(O)C(O)COP(=O)(O)O", "Ru5P") # Ribulose-5-Phosphate
ribuloseP = Ru5P # TODO: remove at some point
F6P = smiles("OCC(=O)C(O)C(O)C(O)COP(=O)(O)O", "F6P") # Fructose-6-Phosphate
fructoseP = F6P # TODO: remove at some point
S7P = smiles("O=P(O)(OCC(O)C(O)C(O)C(O)C(=O)CO)O", "S7P") # Sedoheptulose 7-phosphate
water = smiles("O", "Water")
Pi = smiles("O=P(O)(O)O", "Pi")
phosphate = Pi # TODO: remove at some point

aldoKetoF = ruleGML("aldo_keto_backward.gml")
aldoKetoB = ruleGML("aldo_keto_forward.gml")
transKeto = ruleGML("transketolase.gml")
Tkt = transKeto
transAldo = ruleGML("transaldolase.gml")
Tal = transAldo
aldolase = ruleGML("aldolase.gml")
phosphohydro = ruleGML("phosphohydrolase.gml")

aldolaseReal = leftPredicate[lambda d: any(a.vLabelCount("C") > 2 for a in d.left)](aldolase)

rulesGeneric = [aldoKetoF, aldoKetoB, transKeto, transAldo, aldolase, phosphohydro]
rulesReal = [aldoKetoF, aldoKetoB, transKeto, transAldo, aldolaseReal, phosphohydro]
