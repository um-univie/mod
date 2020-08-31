
import collections
import ctypes
import inspect
import math
import sys

_oldFlags = sys.getdlopenflags()
sys.setdlopenflags(_oldFlags | ctypes.RTLD_GLOBAL)
from .libpymod import *
sys.setdlopenflags(_oldFlags)

# from http://mail.python.org/pipermail/tutor/2003-November/026645.html
class Unbuffered(object):
	def __init__(self, stream):
		self.stream = stream
	def write(self, data):
		self.stream.write(data)
		self.stream.flush()
	def __getattr__(self, attr):
		return getattr(self.stream, attr)
sys.stdout = Unbuffered(sys.stdout)

def _NoNew__setattr__(self, name, value):
	if hasattr(self, "_frozen") and self._frozen:
		msg = "Can not modify object '%s' of type '%s'. It has been frozen." % (self, type(self))
		raise AttributeError(msg)
	if hasattr(self, name):
		object.__setattr__(self, name, value)
	else:
		msg = "Can not create new attribute '%s' on object '%s' of type '%s'." % (name, self, type(self))
		msg += "\ndir(" + str(self) + "):\n"
		for name in dir(self): msg += "\t" + name + "\n"
		raise AttributeError(msg)


def _fixClass(name, c, indent):
	if not name.startswith("Func_"):
		c.__setattr__ = _NoNew__setattr__

	if name.startswith("Func_") or name.startswith("Vec"):
		c.__hash__ = None
	elif name.endswith("Vertex"):
		assert c.__hash__ is not None and c.__hash__ != object.__hash__
	elif issubclass(c, int):
		# Enums fall into this.
		assert c.__hash__ != object.__hash__
	elif c.__hash__ == object.__hash__:
		c.__hash__ = None

	if not (name.startswith("Func_") or name.startswith("Vec")):
		if name.endswith("Vertex") or name.endswith("Edge"):
			assert c.__bool__ is not None

	for a in inspect.getmembers(c, inspect.isclass):
		if a[0] == "__class__": continue
		_fixClass(a[0], a[1], indent + 1)

classes = inspect.getmembers(sys.modules[__name__], inspect.isclass)
for c in classes:
	if c[1] == Unbuffered: continue
	_fixClass(c[0], c[1], 0)

#----------------------------------------------------------

def _deprecation(msg):
	if config.common.ignoreDeprecation:
		print("WARNING: {} Use config.common.ignoreDeprecation = False to make this an exception.".format(msg))
	else:
		raise DeprecationWarning("{} Use config.common.ignoreDeprecation = True to make this just a warning.".format(msg))

#----------------------------------------------------------
# Script Inclusion Support
#----------------------------------------------------------

class CWDPath:
	def __init__(self, f):
		self.f = f

_filePrefixes = []

def prefixFilename(name):
	if isinstance(name, CWDPath):
		return name.f
	if len(name) == 0 or name[0] == '/':
		return name
	prefixed = []
	for s in _filePrefixes:
		if len(s) != 0 and s[0] == '/':
			prefixed[:] = [s]
		else:
			prefixed.append(s)
	prefixed.append(name)
	return ''.join(prefixed)

def pushFilePrefix(prefix):
	_filePrefixes.append(prefix)

def popFilePrefix():
	if len(_filePrefixes) == 0:
		raise LogicError("popFilePrefix failed; the stack is empty")
	_filePrefixes.pop()


#----------------------------------------------------------
# Wrappers
#----------------------------------------------------------

def _wrap(C, l):
	if type(l) is C:
		return l
	lcpp = C()
	lcpp.extend(a for a in l)
	return lcpp
def _unwrap(lcpp):
	l = []
	l.extend(a for a in lcpp)
	return l

def _funcWrap(F, f, resultWrap=None, module=libpymod):
	if isinstance(f, F):
		res = f
	elif hasattr(f, "__call__"):	# wrap functor
		class FuncWrapper(F):
			def __init__(self, f):
				self.f = f
				F.__init__(self)
			def clone(self):
				return module._sharedToStd(FuncWrapper(self.f))
			def __str__(self):
				lines, lnum = inspect.getsourcelines(self.f)
				source = ''.join(lines)
				filename = inspect.getfile(self.f)
				return "FuncWrapper(%s)\nCode from %s:%d >>>>>\n%s<<<<< Code from %s:%d" % (str(self.f), filename, lnum, source, filename, lnum)
			def __call__(self, *args):
				try:
					if resultWrap is not None:
						return _wrap(resultWrap, self.f(*args))
					else:
						return self.f(*args)
				except:
					print("Error in wrapped function when called:", str(self))
					print("Base type is '" + str(F) + "'")
					raise
										
		res = FuncWrapper(f)
	else:							# assume constant
		class Constant(F):
			def __init__(self, c):
				self.c = c
				F.__init__(self)
			def clone(self):
				return module._sharedToStd(Constant(self.c))
			def __str__(self):
				return "Constant(" + str(self.c) + ")"
			def __call__(self, *args):
				return self.c
		res = Constant(f)
	return module._sharedToStd(res)

###########################################################
# Chem
###########################################################

BondType.__str__ = libpymod._bondTypeToString

###########################################################
# Config
###########################################################

LabelType.__str__ = libpymod._LabelType__str__
LabelRelation.__str__ = libpymod._LabelRelation__str__
IsomorphismPolicy.__str__ = libpymod._IsomorphismPolicy__str__
SmilesClassPolicy.__str__ = libpymod._SmilesClassPolicy__str__

config = getConfig()

###########################################################
# Derivation
###########################################################

Derivation.__repr__ = Derivation.__str__

def _Derivation__setattr__(self, name, value):
	if name in ("left", "right"):
		object.__setattr__(self, name, _wrap(VecGraph, value))
	else:
		_NoNew__setattr__(self, name, value)
Derivation.__setattr__ = _Derivation__setattr__


Derivations.__repr__ = Derivations.__str__

def _Derivations__setattr__(self, name, value):
	if name in ("left", "right"):
		object.__setattr__(self, name, _wrap(VecGraph, value))
	elif name == "rules":
		object.__setattr__(self, name, _wrap(VecRule, value))
	else:
		_NoNew__setattr__(self, name, value)
Derivations.__setattr__ = _Derivations__setattr__

###########################################################
# DG
###########################################################

def dgDerivations(ders):
	_deprecation("dgDerivations is deprecated. Use the new build interface.")
	dg = DG()
	with dg.build() as b:
		for d in ders:
			b.addDerivation(d)
	return dg

def dgRuleComp(graphs, strat, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism), ignoreRuleLabelTypes=False):
	_deprecation("dgRuleComp is deprecated. Use the new build interface.")
	dg = DG(labelSettings=labelSettings, graphDatabase=graphs)
	object.__setattr__(dg, "_ruleCompData", {
		"ignoreRuleLabelTypes": ignoreRuleLabelTypes,
		"strat": strat
	})
	def _DG_calc(dg, printInfo=True):
		_deprecation("DG.calc() is deprecated. Use the new build interface.")
		d = dg._ruleCompData
		dg.build().execute(d["strat"], ignoreRuleLabelTypes=d["ignoreRuleLabelTypes"])
	import types
	object.__setattr__(dg, "calc", types.MethodType(_DG_calc, dg))
	return dg

_DG_load_orig = DG.load
def _DG_load(graphDatabase, ruleDatabase, file,
	         graphPolicy=IsomorphismPolicy.Check, verbosity=2):
	return _DG_load_orig(_wrap(VecGraph, graphDatabase),
	                     _wrap(VecRule, ruleDatabase),
	                     prefixFilename(file),
	                     graphPolicy, verbosity)
DG.load = _DG_load

_DG__init__old = DG.__init__
def _DG__init__(self, *,
	            labelSettings=LabelSettings(
	                LabelType.String,
	                LabelRelation.Isomorphism),
	            graphDatabase=[],
	            graphPolicy=IsomorphismPolicy.Check):
	return _DG__init__old(self,
	                      labelSettings,
	                      _wrap(VecGraph, graphDatabase),
	                      graphPolicy)
DG.__init__ = _DG__init__

_DG_print_orig = DG.print
def _DG_print(self, printer=None, data=None):
	if printer is None: printer = DGPrinter()
	if data is None: data = DGPrintData(self)
	return _DG_print_orig(self, data, printer)
DG.print = _DG_print

_DG_findEdge_orig = DG.findEdge
def _DG_findEdge(self, srcsI, tarsI):
	srcs = list(srcsI)
	tars = list(tarsI)
	if len(srcs) == 0: s = None
	elif isinstance(srcs[0], Graph): s = VecGraph
	else: s = VecDGVertex
	if len(tars) == 0: t = None
	elif isinstance(tars[0], Graph): t = VecGraph
	else: t = VecDGVertex
	if s is None and t is None:
		s = VecDGVertex
		t = VecDGVertex
	elif s is None:
		s = t
	elif t is None:
		t = s
	return _DG_findEdge_orig(self, _wrap(s, srcs), _wrap(t, tars))
DG.findEdge = _DG_findEdge

DG.__repr__ = DG.__str__

def _DG__getattribute__(self, name):
	if name == "graphDatabase":
		return _unwrap(self._graphDatabase)
	elif name == "products":
		return _unwrap(self._products)
	else:
		return object.__getattribute__(self, name)
DG.__getattribute__ = _DG__getattribute__

DG.__eq__ = lambda self, other: self.id == other.id
DG.__hash__ = lambda self: self.id


class DGBuildContextManager(object):
	def __init__(self, dg):
		assert dg is not None
		self.dg = dg
		self._builder = _DG_build_orig(self.dg)

	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_val, exc_tb):
		del self._builder
		self.dg = None
		self._builder = None

	def _check(self):
		assert self._builder

	def addDerivation(self, d, graphPolicy=IsomorphismPolicy.Check):
		self._check()
		return self._builder.addDerivation(d, graphPolicy)

	def execute(self, strategy, *, verbosity=2, ignoreRuleLabelTypes=False):
		self._check()
		return self._builder.execute(dgStrat(strategy), verbosity, ignoreRuleLabelTypes)

	def addAbstract(self, description):
		self._check()
		return self._builder.addAbstract(description)
	
	def apply(self, graphs, rule, verbosity=0, graphPolicy=IsomorphismPolicy.Check):
		self._check()
		return _unwrap(self._builder.apply(_wrap(VecGraph, graphs), rule, verbosity, graphPolicy))

	def load(self, ruleDatabase, file, verbosity=2):
		self._check()
		return self._builder.load(
			_wrap(VecRule, ruleDatabase),
			prefixFilename(file), verbosity)

_DG_build_orig = DG.build
DG.build = lambda self: DGBuildContextManager(self)


#----------------------------------------------------------
# DGExecuteResult
#----------------------------------------------------------

def _DGExecuteResult__getattribute__(self, name):
	if name in ("subset", "universe"):
		return _unwrap(object.__getattribute__(self, name))
	return object.__getattribute__(self, name)

DGExecuteResult.__getattribute__ = _DGExecuteResult__getattribute__

_DGExecuteResult_list_old = DGExecuteResult.list
def _DGExecuteResult_list(self, *, withUniverse=False):
	return _DGExecuteResult_list_old(self, withUniverse)
DGExecuteResult.list = _DGExecuteResult_list


#----------------------------------------------------------
# DGHyperEdge
#----------------------------------------------------------

_DGHyperEdge_print_orig = DGHyperEdge.print
DGHyperEdge.print = lambda self, *args, **kwargs: _unwrap(_DGHyperEdge_print_orig(self, *args, **kwargs))


#----------------------------------------------------------
# DGPrinter
#----------------------------------------------------------

def _makeGraphToVertexCallback(orig, name, func):
	def callback(self, f, *args, **kwargs):
		if hasattr(f, "__call__"):
			import inspect
			spec = inspect.getargspec(f)
			if len(spec.args) == 2:
				_deprecation("The callback for {} seems to take two arguments, a graph and a derivation graph. This is deprecated, the callback should take a single DGVertex argument.".format(name))
				fOrig = f
				f = lambda v, fOrig=fOrig: fOrig(v.graph, v.dg)
		return orig(self, _funcWrap(func, f), *args, **kwargs)
	return callback

_DGPrinter_pushVertexVisible_orig = DGPrinter.pushVertexVisible
DGPrinter.pushVertexVisible = _makeGraphToVertexCallback(
	_DGPrinter_pushVertexVisible_orig, "pushVertexVisible", Func_BoolDGVertex)

_DGPrinter_pushEdgeVisible_orig = DGPrinter.pushEdgeVisible
DGPrinter.pushEdgeVisible = \
	lambda self, f: _DGPrinter_pushEdgeVisible_orig(self, _funcWrap(Func_BoolDGHyperEdge, f))

_DGPrinter_pushVertexLabel_orig = DGPrinter.pushVertexLabel
DGPrinter.pushVertexLabel = _makeGraphToVertexCallback(
	_DGPrinter_pushVertexLabel_orig , "pushVertexLabel", Func_StringDGVertex)

_DGPrinter_pushEdgeLabel_orig = DGPrinter.pushEdgeLabel
DGPrinter.pushEdgeLabel = \
	lambda self, f: _DGPrinter_pushEdgeLabel_orig(self, _funcWrap(Func_StringDGHyperEdge, f))

_DGPrinter_pushVertexColour_orig = DGPrinter.pushVertexColour
_DGPrinter_pushVertexColour_inner = _makeGraphToVertexCallback(
	_DGPrinter_pushVertexColour_orig, "pushVertexColour", Func_StringDGVertex)
DGPrinter.pushVertexColour = \
	lambda self, f, extendToEdges=True: _DGPrinter_pushVertexColour_inner(self, f, extendToEdges)

_DGPrinter_pushEdgeColour_orig = DGPrinter.pushEdgeColour
DGPrinter.pushEdgeColour = \
	lambda self, f: _DGPrinter_pushEdgeColour_orig(self, _funcWrap(Func_StringDGHyperEdge, f))

_DGPrinter_setRotationOverwrite_orig = DGPrinter.setRotationOverwrite
DGPrinter.setRotationOverwrite = \
	lambda self, f: _DGPrinter_setRotationOverwrite_orig(self, _funcWrap(Func_IntGraph, f))

_DGPrinter_setMirrorOverwrite_orig = DGPrinter.setMirrorOverwrite
DGPrinter.setMirrorOverwrite = \
	lambda self, f: _DGPrinter_setMirrorOverwrite_orig(self, _funcWrap(Func_BoolGraph, f))


#----------------------------------------------------------
# DGStrat
#----------------------------------------------------------

def _DGStratGraphState__getattribute__(self, name):
	if name == "subset":
		return _unwrap(self._subset)
	elif name == "universe":
		return _unwrap(self._universe)
	else:
		return object.__getattribute__(self, name)
DGStratGraphState.__getattribute__ = _DGStratGraphState__getattribute__

_DGStrat_makeAddStatic_orig = DGStrat.makeAddStatic
def _DGStrat_makeAddStatic(onlyUniverse, graphs, graphPolicy):
	return _DGStrat_makeAddStatic_orig(onlyUniverse, _wrap(VecGraph, graphs), graphPolicy)
DGStrat.makeAddStatic = _DGStrat_makeAddStatic

_DGStrat_makeAddDynamic_orig = DGStrat.makeAddDynamic
def _DGStrat_makeAddDynamic(onlyUniverse, generator, graphPolicy):
	return _DGStrat_makeAddDynamic_orig(onlyUniverse, _funcWrap(Func_VecGraph, generator, resultWrap=VecGraph), graphPolicy)
DGStrat.makeAddDynamic = _DGStrat_makeAddDynamic

_DGStrat_makeSequence_orig = DGStrat.makeSequence
def _DGStrat_makeSequence(l):
	return _DGStrat_makeSequence_orig(_wrap(VecDGStrat, l))
DGStrat.makeSequence = _DGStrat_makeSequence

_DGStrat_makeParallel_orig = DGStrat.makeParallel
def _DGStrat_makeParallel(l):
	return _DGStrat_makeParallel_orig(_wrap(VecDGStrat, l))
DGStrat.makeParallel = _DGStrat_makeParallel

_DGStrat_makeFilter_orig = DGStrat.makeFilter
def _DGStrat_makeFilter(alsoUniverse, filterFunc):
	return _DGStrat_makeFilter_orig(alsoUniverse, _funcWrap(Func_BoolGraphDGStratGraphStateBool, filterFunc))
DGStrat.makeFilter = _DGStrat_makeFilter

_DGStrat_makeExecute_orig = DGStrat.makeExecute
def _DGStrat_makeExecute(func):
	return _DGStrat_makeExecute_orig(_funcWrap(Func_VoidDGStratGraphState, func))
DGStrat.makeExecute = _DGStrat_makeExecute

_DGStrat_makeLeftPredicate_orig = DGStrat.makeLeftPredicate
def _DGStrat_makeLeftPredicate(pred, strat):
	return _DGStrat_makeLeftPredicate_orig(_funcWrap(Func_BoolDerivation, pred), strat)
DGStrat.makeLeftPredicate = _DGStrat_makeLeftPredicate

_DGStrat_makeRightPredicate_orig = DGStrat.makeRightPredicate
def _DGStrat_makeRightPredicate(pred, strat):
	return _DGStrat_makeRightPredicate_orig(_funcWrap(Func_BoolDerivation, pred), strat)
DGStrat.makeRightPredicate = _DGStrat_makeRightPredicate


#----------------------------------------------------------
# Graph
#----------------------------------------------------------

inputGraphs = []

_Graph_print_orig = Graph.print
def _Graph_print(self, first=None, second=None):
	if first is None:
		return _Graph_print_orig(self)
	if second is None:
		second = first
	return _Graph_print_orig(self, first, second)
Graph.print = _Graph_print

_Graph_aut = Graph.aut
Graph.aut = lambda self, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_aut(self, labelSettings)

_Graph_isomorphism = Graph.isomorphism
Graph.isomorphism = lambda self, g, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_isomorphism(self, g, maxNumMatches, labelSettings)
_Graph_monomorphism = Graph.monomorphism
Graph.monomorphism = lambda self, g, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_monomorphism(self, g, maxNumMatches, labelSettings)

_Graph_getGMLString = Graph.getGMLString
Graph.getGMLString = lambda self, withCoords=False: _Graph_getGMLString(self, withCoords)
_Graph_printGML = Graph.printGML
Graph.printGML = lambda self, withCoords=False: _Graph_printGML(self, withCoords)

def _graphLoad(a, name, add):
	if name != None:
		a.name = name
	if add:
		inputGraphs.append(a)
	return a
def graphGMLString(d, name=None, add=True):
	return _graphLoad(libpymod.graphGMLString(d), name, add)
def graphGML(f, name=None, add=True):
	return _graphLoad(libpymod.graphGML(prefixFilename(f)), name, add)
def graphDFS(s, name=None, add=True):
	return _graphLoad(libpymod.graphDFS(s), name, add)
def smiles(s, name=None, add=True, allowAbstract=False, classPolicy=SmilesClassPolicy.NoneOnDuplicate):
	return _graphLoad(libpymod.smiles(s, allowAbstract, classPolicy), name, add)

Graph.__repr__ = lambda self: str(self) + "(" + str(self.id) + ")"
Graph.__eq__ = lambda self, other: self.id == other.id
Graph.__lt__ = lambda self, other: self.id < other.id
Graph.__hash__ = lambda self: self.id

def _Graph__setattr__(self, name, value):
	if name == "image":
		object.__setattr__(self, "image", _funcWrap(Func_String, value))
	else:
		_NoNew__setattr__(self, name, value)
Graph.__setattr__ = _Graph__setattr__

#----------------------------------------------------------
# RCEvaluator
#----------------------------------------------------------

def _RCEvaluator__getattribute__(self, name):
	if name == "ruleDatabase":
		return _unwrap(self._ruleDatabase)
	elif name == "products":
		return _unwrap(self._products)
	else:
		return object.__getattribute__(self, name)
RCEvaluator.__getattribute__ = _RCEvaluator__getattribute__

_RCEvaluator_eval = RCEvaluator.eval
RCEvaluator.eval = lambda self, e, *, verbosity=2: _unwrap(_RCEvaluator_eval(self, e, verbosity))

def rcEvaluator(rules, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism)):
	return libpymod.rcEvaluator(_wrap(VecRule, rules), labelSettings)

#----------------------------------------------------------
# Rule
#----------------------------------------------------------

inputRules = []

_Rule_print_orig = Rule.print
def _Rule_print(self, first=None, second=None):
	if first is None:
		return _Rule_print_orig(self)
	if second is None:
		second = first
	return _Rule_print_orig(self, first, second)
Rule.print = _Rule_print

_Rule_isomorphism = Rule.isomorphism
Rule.isomorphism = lambda self, r, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Rule_isomorphism(self, r, maxNumMatches, labelSettings)
_Rule_monomorphism = Rule.monomorphism
Rule.monomorphism = lambda self, r, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Rule_monomorphism(self, r, maxNumMatches, labelSettings)

_Rule_getGMLString = Rule.getGMLString
Rule.getGMLString = lambda self, withCoords=False: _Rule_getGMLString(self, withCoords)
_Rule_printGML = Rule.printGML
Rule.printGML = lambda self, withCoords=False: _Rule_printGML(self, withCoords)

def ruleGMLString(s, invert=False, add=True):
	a = libpymod.ruleGMLString(s, invert)
	if add:
		inputRules.append(a)
	return a
def ruleGML(f, invert=False, add=True):
	a = libpymod.ruleGML(prefixFilename(f), invert)
	if add:
		inputRules.append(a)
	return a

Rule.__repr__ = lambda self: str(self) + "(" + str(self.id) + ")"
Rule.__eq__ = lambda self, other: self.id == other.id
Rule.__lt__ = lambda self, other: self.id < other.id
Rule.__hash__ = lambda self: self.id

#----------------------------------------------------------
# DG Strategy Prettification
#----------------------------------------------------------

def dgStrat(s):
	if isinstance(s, DGStrat):
		return s
	elif isinstance(s, Rule):
		return DGStrat.makeRule(s)
	elif isinstance(s, _DGStrat_sequenceProxy):
		return DGStrat.makeSequence(s.strats)
	elif isinstance(s, collections.Iterable):
		# do deep dgStrat
		l = [dgStrat(a) for a in s]
		return DGStrat.makeParallel(l)	
	else:
		raise TypeError("Can not convert type '" + str(type(s)) + "' to DGStrat.")

# add
#----------------------------------------------------------

def _DGStrat_add(doUniverse, g, gs, graphPolicy):
	if hasattr(g, "__call__"): # assume the dynamic version is meant
		if len(gs) > 0:
			raise TypeError("The dynamic version of addSubset/addUniverse takes exactly 1 argument (" + str(len(gs) + 1) + " given).")
		return DGStrat.makeAddDynamic(doUniverse, g, graphPolicy)
	else: # assume the static version was meant
		def convertGraphs(graphs, g):
			if isinstance(g, Graph):
				graphs.append(g)
			else:
				graphs.extend(a for a in g)
		graphs=[]
		convertGraphs(graphs, g)
		for a in gs:
			convertGraphs(graphs, a)
		return DGStrat.makeAddStatic(doUniverse, graphs, graphPolicy)
def addUniverse(g, *gs, graphPolicy=IsomorphismPolicy.Check):
	return _DGStrat_add(True, g, gs, graphPolicy)
def addSubset(g, *gs, graphPolicy=IsomorphismPolicy.Check):
	return _DGStrat_add(False, g, gs, graphPolicy)

# derivation predicates
#----------------------------------------------------------

class _DGStrat_DerivationPredicateProxyPredicateHolder(object):
	def __init__(self, isLeft, predicate):
		self.isLeft = isLeft
		self.predicate = predicate
	def __call__(self, strat):
		if self.isLeft:
			return DGStrat.makeLeftPredicate(self.predicate, dgStrat(strat))
		else:
			return DGStrat.makeRightPredicate(self.predicate, dgStrat(strat))

class _DGStrat_DerivationPredicateProxy(object):
	def __init__(self, isLeft):
		self.isLeft = isLeft
	def __getitem__(self, key):
		return _DGStrat_DerivationPredicateProxyPredicateHolder(self.isLeft, key)

leftPredicate = _DGStrat_DerivationPredicateProxy(True)
rightPredicate = _DGStrat_DerivationPredicateProxy(False)

# execute
#----------------------------------------------------------

execute = lambda func: DGStrat.makeExecute(func)

# filter
#----------------------------------------------------------

class _DGStrat_FilterProxy(object):
	def __init__(self, alsoUniverse):
		self.alsoUniverse = alsoUniverse
	def __call__(self, filterFunc):
		return DGStrat.makeFilter(self.alsoUniverse, filterFunc)

filterUniverse = _DGStrat_FilterProxy(True)
filterSubset = _DGStrat_FilterProxy(False)

# repeat
#----------------------------------------------------------

class _DGStrat_RepeatProxyBoundHolder(object):
	def __init__(self, bound):
		self.bound = bound
		if bound < 0:
			raise ArgumentError("The number of repetitions in a repeat strategy must be non-negative. Got '" + str(bound))
	def __call__(self, strat):
		return DGStrat.makeRepeat(self.bound, dgStrat(strat))

class _DGStrat_RepeatProxy(object):
	def __getitem__(self, key):
		return _DGStrat_RepeatProxyBoundHolder(key)
	def __call__(self, strat):
		return self[2**31 - 1](strat)
		
repeat = _DGStrat_RepeatProxy()

# revive
#----------------------------------------------------------

revive = lambda s: DGStrat.makeRevive(dgStrat(s))

# sequence
#----------------------------------------------------------

class _DGStrat_sequenceProxy(object):
	def __init__(self, strats):
		self.strats = strats

def _DGStrat_sequence__rshift__(a, b):
	strats = []
	if isinstance(a, _DGStrat_sequenceProxy):
		strats.extend(s for s in a.strats)
	else:
		strats.append(dgStrat(a))
	if isinstance(b, _DGStrat_sequenceProxy):
		strats.extend(s for s in b.strats)
	else:
		strats.append(dgStrat(b))
	return _DGStrat_sequenceProxy(strats)
DGStrat.__rshift__ = _DGStrat_sequence__rshift__
DGStrat.__rrshift__ = lambda self, other: _DGStrat_sequence__rshift__(other, self)
_DGStrat_sequenceProxy.__rshift__ = _DGStrat_sequence__rshift__
Rule.__rshift__ = _DGStrat_sequence__rshift__


#----------------------------------------------------------
# RCExp prettification
#----------------------------------------------------------

def rcExp(e):
	if isinstance(e, RCExpExp) or isinstance(e, Rule) or isinstance(e, RCExpUnion):
		return e
	elif isinstance(e, RCExpBind) or isinstance(e, RCExpId) or isinstance(e, RCExpUnbind):
		return e
	elif isinstance(e, RCExpComposeCommon) or isinstance(e, RCExpComposeParallel) or isinstance(e, RCExpComposeSub) or isinstance(e, RCExpComposeSuper):
		return e
	elif isinstance(e, collections.Iterable):
		return RCExpUnion(_wrap(VecRCExpExp, [rcExp(a) for a in e]))
	else:
		raise TypeError("Can not convert type '" + str(type(e)) + "' to RCExpExp")

def _rcConvertGraph(g, cls, f):
	if isinstance(g, Graph):
		return cls(g)
	elif isinstance(g, collections.Iterable):
		l = [f(a) for a in g]
		return rcExp(l)
	else:
		raise TypeError("Can not convert type '" + str(type(e)) + "' to " + str(cls))
def rcBind(g):
	return _rcConvertGraph(g, RCExpBind, rcBind)
def rcId(g):
	return _rcConvertGraph(g, RCExpId, rcId)
def rcUnbind(g):
	return _rcConvertGraph(g, RCExpUnbind, rcUnbind)

class _RCCommonOpFirstBound(object):
	def __init__(self, discardNonchemical, maximum, connected, first):
		self.discardNonchemical = discardNonchemical
		self.maximum = maximum
		self.connected = connected
		self.first = first
	def __mul__(self, second):
		return RCExpComposeCommon(rcExp(self.first), rcExp(second), self.discardNonchemical, self.maximum, self.connected)
class _RCCommonOpArgsBound(object):
	def __init__(self, discardNonchemical, maximum, connected):
		self.discardNonchemical = discardNonchemical
		self.maximum = maximum
		self.connected = connected
	def __rmul__(self, first):
		return _RCCommonOpFirstBound(self.discardNonchemical, self.maximum, self.connected, first)
class _RCCommonOp(object):
	def __call__(self, discardNonchemical=True, maximum=False, connected=True):
		return _RCCommonOpArgsBound(discardNonchemical, maximum, connected)
	def __rmul__(self, first):
		return first * self()
rcCommon = _RCCommonOp()

class _RCParallelOpFirstBound(object):
	def __init__(self, discardNonchemical, first):
		self.discardNonchemical = discardNonchemical
		self.first = first
	def __mul__(self, second):
		return RCExpComposeParallel(rcExp(self.first), rcExp(second), self.discardNonchemical)
class _RCParallelOpArgsBound(object):
	def __init__(self, discardNonchemical):
		self.discardNonchemical = discardNonchemical
	def __rmul__(self, first):
		return _RCParallelOpFirstBound(self.discardNonchemical, first)
class _RCParallelOp(object):
	def __call__(self, discardNonchemical=True):
		return _RCParallelOpArgsBound(discardNonchemical)
	def __rmul__(self, first):
		return first * self()
rcParallel = _RCParallelOp()

class _RCSubOpFirstBound(object):
	def __init__(self, discardNonchemical, allowPartial, first):
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.first = first
	def __mul__(self, second):
		return RCExpComposeSub(rcExp(self.first), rcExp(second), self.discardNonchemical, self.allowPartial)
class _RCSubOpArgsBound(object):
	def __init__(self, discardNonchemical, allowPartial):
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
	def __rmul__(self, first):
		return _RCSubOpFirstBound(self.discardNonchemical, self.allowPartial, first)
class _RCSubOp(object):
	def __call__(self, discardNonchemical=True, allowPartial=True):
		return _RCSubOpArgsBound(discardNonchemical, allowPartial)
	def __rmul__(self, first):
		return first * self()
rcSub = _RCSubOp()

class _RCSuperOpFirstBound(object):
	def __init__(self, discardNonchemical, allowPartial, enforceConstraints, first):
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.enforceConstraints = enforceConstraints
		self.first = first
	def __mul__(self, second):
		return RCExpComposeSuper(rcExp(self.first), rcExp(second), self.discardNonchemical, self.allowPartial, self.enforceConstraints)
class _RCSuperOpArgsBound(object):
	def __init__(self, discardNonchemical, allowPartial, enforceConstraints):
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.enforceConstraints = enforceConstraints
	def __rmul__(self, first):
		return _RCSuperOpFirstBound(self.discardNonchemical, self.allowPartial, self.enforceConstraints, first)
class _RCSuperOp(object):
	def __call__(self, discardNonchemical=True, allowPartial=True, enforceConstraints=False):
		return _RCSuperOpArgsBound(discardNonchemical, allowPartial, enforceConstraints)
	def __rmul__(self, first):
		return first * self()
rcSuper = _RCSuperOp()

#----------------------------------------------------------
# Util
#----------------------------------------------------------

def showDump(f):
	return libpymod.showDump(prefixFilename(f))

def boltzmannFactor(temperature, energy):
	return math.exp(-energy / temperature)

class Boltzmann(object):
	def __init__(self, temperature):
		self.temperature = temperature
		self.col = None
	def __call__(self, g, col, first):
		if first:
			self.col = col
			self.sumFactors = sum(boltzmannFactor(self.temperature, a.energy) for a in col)
		return boltzmannFactor(self.temperature, g.energy) / self.sumFactors

class BoltzmannGroupIsomers(object):
	def __init__(self, temperature):
		self.temperature = temperature
		self.col = None
	def __call__(self, g, col, first):
		class Data(dict):
			def __missing__(self, key):
				return 0
		class Histogram(object):
			def __init__(self, g):
				self.counts = {}
				labels = ["C", "N", "H", "O", "c", "n", "O-"]
				for l in labels:
					self.counts[l] = g.vLabelCount(l)
				if g.numVertices != sum(self.counts.values()):
					print("Graph label histogram incomplete!")
					print("Graph is", g)
					g.print()
					sys.exit(0)
				self.counts = frozenset(self.counts.items())
			def __hash__(self):
				return hash(self.counts)
			def __eq__(self, other):
				return self.counts == other.counts
		if first:
			self.col = col
			self.groups = Data()
			for a in col:
				self.groups[Histogram(a)] += boltzmannFactor(self.temperature, a.energy)
		return boltzmannFactor(self.temperature, g.energy) / self.groups[Histogram(g)]


#class BoltzmannGroupWeight(object):
#	def __init__(self, temperature):
#		self.temperature = temperature
#		self.col = None
#	def __call__(self, g, col, first):
#		class Data(dict):
#			def __missing__(self, key):
#				assert(key == int(key))
#				return 0
#		if first:
#			self.col = col
#			self.sums = Data()
#			for a in col:
#				weight = int(a.weight)
#				self.sums[weight] += boltzmannFactor(self.temperature, a.energy)
#		weight = int(g.weight)
#		return boltzmannFactor(self.temperature, g.energy) / self.sums[weight]

