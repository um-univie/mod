include("problematic.py")
include("smiles_cansmi_roundtrip.py")
include("smiles_nci.py")
config.graph.smilesCheckAST = False
include("smilesMetacycReactionSmiles.py")
config.graph.smilesCheckAST = True
include("rhea.py")
