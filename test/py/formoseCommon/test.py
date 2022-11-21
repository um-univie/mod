include("grammar.py")

r1 = ketoEnol_F
r2 = aldolAdd_F

c1_1K = r1.getVertexFromExternalId(1)
c1_2K = r1.getVertexFromExternalId(2)
c2_1K = r2.getVertexFromExternalId(1)
c2_2K = r2.getVertexFromExternalId(2)
o2_3K = r2.getVertexFromExternalId(3)

m = RCMatch(r1, r2)
print("hmm:", c1_1K.right.id, c2_2K.left.id)
m.push(c1_1K.right, c2_2K.left)
print("hmm:", c1_2K.right.id, c2_1K.left.id)
m.push(c1_2K.right, c2_1K.left)
res = m.compose()
res.print() 

for a in inputRules:
	a.print()
