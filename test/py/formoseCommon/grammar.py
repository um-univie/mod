formaldehyde = smiles("C=O", "Formaldehyde")
glycolaldehyde = smiles( "OCC=O", "Glycolaldehyde")
ketoEnol_F = ruleGML("keto_enol_forward.gml")
ketoEnol_B = ruleGML("keto_enol_backward.gml")
aldolAdd_F = ruleGML("aldol_addition_forward.gml")
aldolAdd_B = ruleGML("aldol_addition_backward.gml")