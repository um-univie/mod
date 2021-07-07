include("common.py")

print("Explicit -------------------------------------------------------------")
handleExp(RCExpComposeCommon(rcExp(ketoEnol_F), rcExp(aldolAdd_F), True, True, False, False))
print("Semi-implicit --------------------------------------------------------")
handleExp(rcExp(ketoEnol_F) * rcCommon * rcExp(aldolAdd_F))
print("Implicit -------------------------------------------------------------")
handleExp(ketoEnol_F * rcCommon() * aldolAdd_F)
print("Implicit 2 -----------------------------------------------------------")
handleExp(ketoEnol_F * rcCommon * aldolAdd_F)

post("disableSummary")

rc = rcEvaluator(inputRules)
resWithout = rc.eval(ketoEnol_F *rcCommon* ketoEnol_B)
resWith = rc.eval(ketoEnol_F *rcCommon(includeEmpty=True)* ketoEnol_B)
print("|resWithout|:", len(resWithout))
print("|resWith|:", len(resWith))
assert len(resWithout) + 1 == len(resWith)
