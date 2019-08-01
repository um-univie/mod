include("common.py")
printGeometryGraph()

data = []
data.append(('any', dfs("[Q]")))
data.append(('any', dfs("[H+]")))
data.append(('any', dfs("[H][Z]")))
data.append(('any', dfs("[Z][Q][Z]")))
data.append(('linearFree', dfs("[Z]#C[Z]")))
data.append(('linear90', dfs("[Z]=C=[Z]")))
data.append(('any', dfs("[Z][Q]([Z])[Z]")))
data.append(('trigonalPlanar', dfs("[Z]=C([Z])[Z]")))
data.append(('trigonalPlanar', dfs("[Z]:C(:[Z]):[Z]")))
data.append(('trigonalPlanar', dfs("[Z]:N([Z]):[Z]")))
data.append(('trigonalPlanar', dfs("[Z]:N:[Z]")))
data.append(('any', dfs("[Z][Q]([Z])([Z])[Z]")))
data.append(('tetrahedral', dfs("[Z]C([Z])([Z])[Z]")))
data.append(('tetrahedral', dfs("[Z]N([Z])[Z]")))
data.append(('tetrahedral', dfs("[Z][N+]([Z])([Z])[Z]")))
data.append(('tetrahedral', dfs("[Z]=P([Z])([Z])[Z]")))
data.append(('tetrahedral', dfs("[Z]P(=[Z])([Z])[Z]"))) # for testing initialisation
data.append(('tetrahedral', dfs("[Z]P([Z])(=[Z])[Z]"))) # for testing initialisation
data.append(('tetrahedral', dfs("[Z]P([Z])([Z])=[Z]"))) # for testing initialisation

for side in ["context", "left", "right"]:
	postChapter("Rule " + side)
	for n, a in data:
		postSection(n + " " + side)
		gml = a.getGMLString()
		gml = gml[7:-2]
		rGML(gml, side)
