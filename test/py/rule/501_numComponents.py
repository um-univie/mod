include("xxx_helpers.py")

def c(dfs, numL, numR):
	r = Rule.fromDFS(dfs)
	assert r.numLeftComponents == numL
	assert r.numRightComponents == numR
	commonChecks(r)

# L
c("[X]>>", 1, 0)
c("[X][Y]>>", 1, 0)
c("[X].[Y]>>", 2, 0)

# K
c("[X]1>>[X]1", 1, 1)
c("[X]1[Y]2>>[X]1[Y]2", 1, 1)
c("[X]1.[Y]2>>[X]1.[Y]2", 2, 2)

# R
c(">>[X]", 0, 1)
c(">>[X][Y]", 0, 1)
c(">>[X].[Y]", 0, 2)
