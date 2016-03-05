import mod
import os
import sys

_texFile = None
_figFolder = None
_graphs = []
_rules = []

def setTexFile(fName):
	global _texFile
	_texFile = open(fName, "w")
	mod.post("disableSummary")

def setFigFolder(fName):
	global _figFolder
	_figFolder = fName
	_checkSettings()
	mod.post("post \"mkdir -p '%s'\"" % _figFolder)

def _checkSettings():
	if _texFile is None:
		print("Error: no tex file set")
		sys.exit(1)
	if _figFolder is None or _figFolder == "":
		print("Error: no figure folder set, or empty name")
		sys.exit(1)

def outputFile(f):
	_checkSettings()
	mod.post("post \"cp '%s' '%s/'\"" % (f, _figFolder))
	return _figFolder + "/" +  os.path.basename(f)

def texDefine(id, value):
	_checkSettings()
	_texFile.write(r"\expandafter\def\csname mod@figDef-%s\endcsname{%s}"
		% (str(id), str(value)))
	_texFile.write("\n")

#------------------------------------------------------------------------------
# Grpahs
#------------------------------------------------------------------------------

def graph(id, g, p):
	_checkSettings()
	for a in _graphs:
		if a.isomorphism(g, 1) == 1:
			g = a
			break
	else:
		_graphs.append(g)
	f = g.print(p, p)	
	f = f[0]
	f = outputFile(f)
	texDefine("graph-" + str(id), f)

def graphGML(id, data, printer):
	graph(id, mod.graphGML(data), printer)

def smiles(id, data, printer):
	graph(id, mod.smiles(data.replace('##', '#')), printer)

def graphDFS(id, data, printer):
	graph(id, mod.graphDFS(data.replace('##', '#')), printer)

#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

def rule(id, r, p):
	_checkSettings()
	for a in _rules:
		if a.isomorphism(r, 1) == 1:
			r = a
			break
	else:
		_rules.append(r)
	f = r.print(p, p)
	f = f[0]
	fL = outputFile(f + ".L.pdf")
	fK = outputFile(f + ".K.pdf")
	fR = outputFile(f + ".R.pdf")
	texDefine("rule-" + str(id), "{%s}{%s}{%s}" % (fL, fK, fR))

def ruleGML(id, data, printer):
	rule(id, mod.ruleGML(data), printer)
