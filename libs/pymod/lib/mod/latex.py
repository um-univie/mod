import mod
import os
import sys

from typing import List

_texFile = None
_figFolder = None
_graphs: List[mod.Graph] = []
_rules: List[mod.Rule] = []
_ls = mod.LabelSettings(mod.LabelType.String, mod.LabelRelation.Isomorphism,
	mod.LabelRelation.Isomorphism)


def setTexFile(fName):
	global _texFile
	_texFile = open(fName, "w")
	mod.post.disableCompileSummary()


def setFigFolder(fName):
	global _figFolder
	_figFolder = fName
	_checkSettings()
	mod.post.command("post \"mkdir -p '%s'\"" % _figFolder)


def _checkSettings():
	if _texFile is None:
		print("Error: no tex file set")
		sys.exit(1)
	if _figFolder is None or _figFolder == "":
		print("Error: no figure folder set, or empty name")
		sys.exit(1)


def outputFile(f, inline=False):
	_checkSettings()
	assert f.endswith(".pdf")
	f = f[:-4]
	f += ".tex" if inline else ".pdf"
	mod.post.command("post \"cp '%s' '%s/'\"" % (f, _figFolder))
	res = _figFolder + "/" + os.path.basename(f)
	return res[:-4]


def texDefine(id, value):
	_checkSettings()
	_texFile.write(r"\expandafter\def\csname mod@figDef-%s\endcsname{%s}"
		% (str(id), str(value)))
	_texFile.write("\n")


# ------------------------------------------------------------------------------
# Grpahs
# ------------------------------------------------------------------------------

def graph(id, g, p, inline):
	_checkSettings()
	for a in _graphs:
		if a.isomorphism(g, 1, labelSettings=_ls) == 1:
			g = a
			break
	else:
		_graphs.append(g)
	f = g.print(p, p)
	f = f[0]
	f = outputFile(f, inline)
	texDefine("graph-" + str(id), f)


def graphGML(id, data, printer, inline=False):
	graph(id, mod.Graph.fromGMLFile(data), printer, inline)


def smiles(id, data, printer, inline=False):
	graph(id, mod.Graph.fromSMILES(data.replace('##', '#')), printer, inline)


def graphDFS(id, data, printer, inline=False):
	graph(id, mod.Graph.fromDFS(data.replace('##', '#')), printer, inline)


# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------

def rule(id, r, p):
	_checkSettings()
	for a in _rules:
		if a.isomorphism(r, 1, labelSettings=_ls) == 1:
			r = a
			break
	else:
		_rules.append(r)
	f = r.print(p, p)
	f = f[0]
	fL = outputFile(f + "_L.pdf")
	fK = outputFile(f + "_K.pdf")
	fR = outputFile(f + "_R.pdf")
	texDefine("rule-" + str(id), "{%s}{%s}{%s}" % (fL, fK, fR))


def ruleGML(id, data, printer):
	rule(id, mod.Rule.fromGMLFile(data), printer)
