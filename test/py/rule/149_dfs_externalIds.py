include("xxx_helpers.py")

r = Rule.fromDFS("[A]1[B]2>>[A]1[C]3")

v1 = r.getVertexFromExternalId(1)
assert v1
assert v1.left
assert v1.left.stringLabel == "A"
assert v1.right
assert v1.right.stringLabel == "A"

v2 = r.getVertexFromExternalId(2)
assert v2
assert v2.left
assert v2.left.stringLabel == "B"
assert not v2.right

v3 = r.getVertexFromExternalId(3)
assert v3
assert not v3.left
assert v3.right
assert v3.right.stringLabel == "C"

commonChecks(r)
