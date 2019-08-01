include("../formoseCommon/grammar.py")
include("common.py")

print("===> seq 2")
handleDG(addUniverse(formaldehyde) >> addSubset(glycolaldehyde))

print("===> seq 3 left")
handleDG(addUniverse(formaldehyde) >> addSubset(glycolaldehyde) >> addSubset(formaldehyde))

print("===> seq 3 right")
handleDG(addUniverse(formaldehyde) >> (addSubset(glycolaldehyde) >> addSubset(formaldehyde)))
