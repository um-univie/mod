include("xxx_helpers.py")

data = "[L]1>>[R]1"

inputRules[:] = []
r1 = Rule.fromDFS(data)
assert inputRules == [r1]
r2 = Rule.fromDFS(data)
assert inputRules == [r1, r2]
Rule.fromDFS(data, add=False)
assert inputRules == [r1, r2]
