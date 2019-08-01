include("common.py")

print("Explicit --------------------------------------------------------------------")
handleExp(rcExp({ketoEnol_F, ketoEnol_B}))

print("Implicit --------------------------------------------------------------------")
handleExp({ketoEnol_F, ketoEnol_B})

print("Input rules -----------------------------------------------------------------")
handleExp(inputRules)
