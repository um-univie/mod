include("../stereoIsoCommon.py")

smiles("C(C(C(=O)O)O)(C(=O)O)O", name="Tartaric acid")
smiles("[C@@H]([C@H](C(=O)O)O)(C(=O)O)O", name="L-tartaric acid")
smiles("[C@H]([C@@H](C(=O)O)O)(C(=O)O)O", name="D-tartaric acid")
smiles("[C@@H]([C@@H](C(=O)O)O)(C(=O)O)O", name="Meso-tartaric acid")
dg = dgRuleComp(inputGraphs, addSubset(inputGraphs) >> repeat(inputRules),
	labelSettings=LabelSettings(LabelType.Term, LabelRelation.Specialisation, LabelRelation.Specialisation))
dg.calc()
p = DGPrinter()
p.withRuleName = True
p.withRuleId = False
dg.print(p)
