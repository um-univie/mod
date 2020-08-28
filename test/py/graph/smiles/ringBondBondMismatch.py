include("common.py")

fail(lambda: smiles("C-1CCCC=1"), "Error in SMILES conversion: ring closure 1 can not be both '-' and '='.")
