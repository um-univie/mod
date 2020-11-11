
import collections
import ctypes
import inspect
import math
import sys
from typing import (
	Any, Callable, cast, Generic, Iterable, List, Optional, Sequence, Tuple,
	Type, TypeVar, Union
)

_oldFlags = sys.getdlopenflags()
sys.setdlopenflags(_oldFlags | ctypes.RTLD_GLOBAL)
from . import libpymod
from .libpymod import *
sys.setdlopenflags(_oldFlags)

# from http://mail.python.org/pipermail/tutor/2003-November/026645.html
class _Unbuffered:
	def __init__(self, stream):
		self.stream = stream
	def write(self, data):
		self.stream.write(data)
		self.stream.flush()
	def __getattr__(self, attr):
		return getattr(self.stream, attr)
sys.stdout = _Unbuffered(sys.stdout)  # type: ignore

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


def _fixClass(name, c, indent) -> None:
	if not name.startswith("_Func_"):
		c.__setattr__ = _NoNew__setattr__

	if name.startswith("_Func_") or name.startswith("_Vec"):
		c.__hash__ = None
	elif name.endswith("Vertex"):
		assert c.__hash__ is not None and c.__hash__ != object.__hash__
	elif issubclass(c, int):
		# Enums fall into this.
		assert c.__hash__ != object.__hash__
	elif c.__hash__ == object.__hash__:
		c.__hash__ = None

	if not (name.startswith("_Func_") or name.startswith("_Vec")):
		if name.endswith("Vertex") or name.endswith("Edge"):
			assert c.__bool__ is not None

	for a in inspect.getmembers(c, inspect.isclass):
		if a[0] == "__class__": continue
		_fixClass(a[0], a[1], indent + 1)

classes = inspect.getmembers(libpymod, inspect.isclass)
for c in classes:
	_fixClass(c[0], c[1], 0)

#----------------------------------------------------------

def _deprecation(msg) -> None:
	if config.common.ignoreDeprecation:
		print("WARNING: {} Use config.common.ignoreDeprecation = False to make this an exception.".format(msg))
	else:
		raise DeprecationWarning("{} Use config.common.ignoreDeprecation = True to make this just a warning.".format(msg))

#----------------------------------------------------------
# Script Inclusion Support
#----------------------------------------------------------

class CWDPath:
	def __init__(self, f) -> None:
		self.f = f

_filePrefixes: List[str] = []

def prefixFilename(name: str) -> str:
	if isinstance(name, CWDPath):
		return name.f
	if len(name) == 0 or name[0] == '/':
		return name
	prefixed: List[str] = []
	for s in _filePrefixes:
		if len(s) != 0 and s[0] == '/':
			prefixed[:] = [s]
		else:
			prefixed.append(s)
	prefixed.append(name)
	return ''.join(prefixed)

def pushFilePrefix(prefix: str) -> None:
	_filePrefixes.append(prefix)

def popFilePrefix() -> None:
	if len(_filePrefixes) == 0:
		raise LogicError("popFilePrefix failed; the stack is empty")
	_filePrefixes.pop()


#----------------------------------------------------------
# Wrappers
#----------------------------------------------------------

def _wrap(C: Type["Vec[T]"], l: Union["Vec[T]", Iterable["T"]]) -> "Vec[T]":
	if isinstance(l, C):
		return l
	lcpp = C()
	lcpp.extend(l)
	return lcpp
def _unwrap(lcpp: Iterable["T"]) -> List["T"]:
	l: List[T] = []
	l.extend(lcpp)
	return l

def _funcWrap(F: Type["U"], f: Union["T", Callable], resultWrap: Optional[Type["Vec[T]"]] = None, module=libpymod) -> "U":
	if hasattr(f, "__call__"):
		class FuncWrapper(F):  # type: ignore
			def __init__(self, f) -> None:
				self.f = f
				F.__init__(self)
			def clone(self) -> "FuncWrapper":
				return module._sharedToStd(FuncWrapper(self.f))
			def __str__(self) -> str:
				lines, lnum = inspect.getsourcelines(self.f)
				source = ''.join(lines)
				filename = inspect.getfile(self.f)
				return "FuncWrapper(%s)\nCode from %s:%d >>>>>\n%s<<<<< Code from %s:%d" % (str(self.f), filename, lnum, source, filename, lnum)
			def __call__(self, *args) -> Union["T", "Vec[T]"]:
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
		class Constant(F):  # type: ignore
			def __init__(self, c) -> None:
				self.c = c
				F.__init__(self)
			def clone(self) -> "Constant":
				return module._sharedToStd(Constant(self.c))
			def __str__(self) -> str:
				return "Constant(" + str(self.c) + ")"
			def __call__(self, *args) -> "T":
				return self.c
		res = Constant(f)
	return module._sharedToStd(res)


###########################################################
# Chem
###########################################################

BondType.__str__ = libpymod._bondTypeToString  # type: ignore


###########################################################
# Config
###########################################################

LabelType.__str__ = libpymod._LabelType__str__  # type: ignore
LabelRelation.__str__ = libpymod._LabelRelation__str__  # type: ignore
IsomorphismPolicy.__str__ = libpymod._IsomorphismPolicy__str__  # type: ignore
SmilesClassPolicy.__str__ = libpymod._SmilesClassPolicy__str__  # type: ignore

config = getConfig()


###########################################################
# Derivation
###########################################################

Derivation.__repr__ = Derivation.__str__  # type: ignore

def _Derivation__setattr__(self, name, value) -> None:
	if name in ("left", "right"):
		object.__setattr__(self, name, _wrap(libpymod._VecGraph, value))
	else:
		_NoNew__setattr__(self, name, value)
Derivation.__setattr__ = _Derivation__setattr__  # type: ignore


Derivations.__repr__ = Derivations.__str__  # type: ignore

def _Derivations__setattr__(self, name, value) -> None:
	if name in ("left", "right"):
		object.__setattr__(self, name, _wrap(libpymod._VecGraph, value))
	elif name == "rules":
		object.__setattr__(self, name, _wrap(libpymod._VecRule, value))
	else:
		_NoNew__setattr__(self, name, value)
Derivations.__setattr__ = _Derivations__setattr__  # type: ignore


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
def _DG_load(
		graphDatabase: List[Graph], ruleDatabase: List[Rule], file: str,
		graphPolicy: IsomorphismPolicy = IsomorphismPolicy.Check,
		verbosity: int = 2) -> DG:
	return _DG_load_orig(
		_wrap(libpymod._VecGraph, graphDatabase),
		_wrap(libpymod._VecRule, ruleDatabase),
		prefixFilename(file), graphPolicy, verbosity)
DG.load = _DG_load  # type: ignore

_DG__init__old = DG.__init__
def _DG__init__(self, *,
		labelSettings: LabelSettings = LabelSettings(
			LabelType.String,
			LabelRelation.Isomorphism),
		graphDatabase: List[Graph] = [],
		graphPolicy: IsomorphismPolicy = IsomorphismPolicy.Check) -> None:
	return _DG__init__old(self,  # type: ignore
	                      labelSettings,
	                      _wrap(libpymod._VecGraph, graphDatabase),
	                      graphPolicy)
DG.__init__ = _DG__init__  # type: ignore

_DG_print_orig = DG.print
def _DG_print(self, printer: Optional[DGPrinter] = None, data: Optional[DGPrintData] = None) -> Tuple[str, str]:
	if printer is None: printer = DGPrinter()
	if data is None: data = DGPrintData(self)
	return _DG_print_orig(self, printer, data)
DG.print = _DG_print  # type: ignore

_DG_findEdge_orig = DG.findEdge
def _DG_findEdge(self,
		srcsI: Union[Sequence[Graph], Sequence[DGVertex]],
		tarsI: Union[Sequence[Graph], Sequence[DGVertex]]) -> DGHyperEdge:
	srcs = srcsI
	tars = tarsI

	s: Union[None, Type[libpymod._VecGraph], Type[libpymod._VecDGVertex]]
	t: Union[None, Type[libpymod._VecGraph], Type[libpymod._VecDGVertex]]

	if len(srcs) == 0:
		s = None
	elif isinstance(srcs[0], Graph):
		s = libpymod._VecGraph
	else:
		s = libpymod._VecDGVertex

	if len(tars) == 0:
		t = None
	elif isinstance(tars[0], Graph):
		t = libpymod._VecGraph
	else:
		t = libpymod._VecDGVertex

	if s is None and t is None:
		s = libpymod._VecDGVertex
		t = libpymod._VecDGVertex
	elif s is None:
		s = t
	elif t is None:
		t = s
	return _DG_findEdge_orig(self, _wrap(s, srcs), _wrap(t, tars))  # type: ignore
DG.findEdge = _DG_findEdge  # type: ignore

DG.__repr__ = DG.__str__  # type: ignore

def _DG__getattribute__(self, name):
	if name == "graphDatabase":
		return _unwrap(self._graphDatabase)
	elif name == "products":
		return _unwrap(self._products)
	else:
		return object.__getattribute__(self, name)
DG.__getattribute__ = _DG__getattribute__  # type: ignore

DG.__eq__ = lambda self, other: self.id == other.id  # type: ignore
DG.__hash__ = lambda self: self.id  # type: ignore


class DGBuildContextManager:
	dg: Optional[DG]
	_builder: Optional[DGBuilder]

	def __init__(self, dg: DG) -> None:
		assert dg is not None
		self.dg = dg
		self._builder = _DG_build_orig(self.dg)

	def __enter__(self) -> "DGBuildContextManager":
		return self

	def __exit__(self, exc_type, exc_val, exc_tb) -> None:
		del self._builder
		self.dg = None
		self._builder = None

	def addDerivation(self, d: Derivations,
			graphPolicy: IsomorphismPolicy = IsomorphismPolicy.Check) -> DGHyperEdge:
		assert self._builder
		return self._builder.addDerivation(d, graphPolicy)

	def execute(self, strategy, *, verbosity=2, ignoreRuleLabelTypes=False) -> DGExecuteResult:
		assert self._builder
		return self._builder.execute(dgStrat(strategy), verbosity, ignoreRuleLabelTypes)  # type: ignore
	
	def apply(self, graphs, rule, onlyProper=True, verbosity=0, graphPolicy=IsomorphismPolicy.Check) -> List[DGHyperEdge]:
		assert self._builder
		return _unwrap(self._builder.apply(_wrap(libpymod._VecGraph, graphs), rule, onlyProper, verbosity, graphPolicy))

	def apply2(self, graphs, rule, verbosity=0, graphPolicy=IsomorphismPolicy.Check) -> List[DGHyperEdge]:
		assert self._builder
		return _unwrap(self._builder.apply2(_wrap(libpymod._VecGraph, graphs), rule))

	def addAbstract(self, description: str) -> None:
		assert self._builder
		return self._builder.addAbstract(description)

	def load(self, ruleDatabase: List[Rule], file: str, verbosity: int = 2) -> None:
		assert self._builder
		return self._builder.load(
			_wrap(libpymod._VecRule, ruleDatabase),
			prefixFilename(file), verbosity)

_DG_build_orig = DG.build
DG.build = lambda self: DGBuildContextManager(self)  # type: ignore


#----------------------------------------------------------
# DGExecuteResult
#----------------------------------------------------------

def _DGExecuteResult__getattribute__(self, name):
	if name in ("subset", "universe"):
		return _unwrap(object.__getattribute__(self, name))
	return object.__getattribute__(self, name)

DGExecuteResult.__getattribute__ = _DGExecuteResult__getattribute__  # type: ignore

_DGExecuteResult_list_old = DGExecuteResult.list
def _DGExecuteResult_list(self, *, withUniverse=False) -> None:
	return _DGExecuteResult_list_old(self, withUniverse)  # type: ignore
DGExecuteResult.list = _DGExecuteResult_list  # type: ignore


#----------------------------------------------------------
# DGHyperEdge
#----------------------------------------------------------

_DGHyperEdge_print_orig = DGHyperEdge.print
DGHyperEdge.print = lambda self, *args, **kwargs: _unwrap(_DGHyperEdge_print_orig(self, *args, **kwargs))  # type: ignore


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
DGPrinter.pushVertexVisible = _makeGraphToVertexCallback(  # type: ignore
	_DGPrinter_pushVertexVisible_orig, "pushVertexVisible", libpymod._Func_BoolDGVertex)

_DGPrinter_pushEdgeVisible_orig = DGPrinter.pushEdgeVisible
DGPrinter.pushEdgeVisible = (  # type: ignore
	lambda self, f: _DGPrinter_pushEdgeVisible_orig(self, _funcWrap(libpymod._Func_BoolDGHyperEdge, f)))

_DGPrinter_pushVertexLabel_orig = DGPrinter.pushVertexLabel
DGPrinter.pushVertexLabel = _makeGraphToVertexCallback(  # type: ignore
	_DGPrinter_pushVertexLabel_orig , "pushVertexLabel", libpymod._Func_StringDGVertex)

_DGPrinter_pushEdgeLabel_orig = DGPrinter.pushEdgeLabel
DGPrinter.pushEdgeLabel = (  # type: ignore
	lambda self, f: _DGPrinter_pushEdgeLabel_orig(self, _funcWrap(libpymod._Func_StringDGHyperEdge, f)))

_DGPrinter_pushVertexColour_orig = DGPrinter.pushVertexColour
_DGPrinter_pushVertexColour_inner = _makeGraphToVertexCallback(
	_DGPrinter_pushVertexColour_orig, "pushVertexColour", libpymod._Func_StringDGVertex)
DGPrinter.pushVertexColour = (  # type: ignore
	lambda self, f, extendToEdges=True: _DGPrinter_pushVertexColour_inner(self, f, extendToEdges))

_DGPrinter_pushEdgeColour_orig = DGPrinter.pushEdgeColour
DGPrinter.pushEdgeColour = (  # type: ignore
	lambda self, f: _DGPrinter_pushEdgeColour_orig(self, _funcWrap(libpymod._Func_StringDGHyperEdge, f)))

_DGPrinter_setRotationOverwrite_orig = DGPrinter.setRotationOverwrite
DGPrinter.setRotationOverwrite = (  # type: ignore
	lambda self, f: _DGPrinter_setRotationOverwrite_orig(self, _funcWrap(libpymod._Func_IntGraph, f)))

_DGPrinter_setMirrorOverwrite_orig = DGPrinter.setMirrorOverwrite
DGPrinter.setMirrorOverwrite = (  # type: ignore
	lambda self, f: _DGPrinter_setMirrorOverwrite_orig(self, _funcWrap(libpymod._Func_BoolGraph, f)))


#----------------------------------------------------------
# Strategy
#----------------------------------------------------------

def _DGStratGraphState__getattribute__(self, name):
	if name == "subset":
		return _unwrap(self._subset)
	elif name == "universe":
		return _unwrap(self._universe)
	else:
		return object.__getattribute__(self, name)
DGStrat.GraphState.__getattribute__ = _DGStratGraphState__getattribute__  # type: ignore

_DGStrat_makeAddStatic_orig = DGStrat.makeAddStatic
def _DGStrat_makeAddStatic(onlyUniverse, graphs, graphPolicy) -> DGStrat:
	return _DGStrat_makeAddStatic_orig(onlyUniverse, _wrap(libpymod._VecGraph, graphs), graphPolicy)
DGStrat.makeAddStatic = _DGStrat_makeAddStatic  # type: ignore

_DGStrat_makeAddDynamic_orig = DGStrat.makeAddDynamic
def _DGStrat_makeAddDynamic(onlyUniverse, generator, graphPolicy) -> DGStrat:
	return _DGStrat_makeAddDynamic_orig(onlyUniverse, _funcWrap(libpymod._Func_VecGraph, generator, resultWrap=libpymod._VecGraph), graphPolicy)
DGStrat.makeAddDynamic = _DGStrat_makeAddDynamic  # type: ignore

_DGStrat_makeSequence_orig = DGStrat.makeSequence
def _DGStrat_makeSequence(l) -> DGStrat:
	return _DGStrat_makeSequence_orig(_wrap(libpymod._VecDGStrat, l))
DGStrat.makeSequence = _DGStrat_makeSequence  # type: ignore

_DGStrat_makeParallel_orig = DGStrat.makeParallel
def _DGStrat_makeParallel(l) -> DGStrat:
	return _DGStrat_makeParallel_orig(_wrap(libpymod._VecDGStrat, l))
DGStrat.makeParallel = _DGStrat_makeParallel  # type: ignore

_DGStrat_makeFilter_orig = DGStrat.makeFilter
def _DGStrat_makeFilter(alsoUniverse, filterFunc) -> DGStrat:
	return _DGStrat_makeFilter_orig(alsoUniverse, _funcWrap(libpymod._Func_BoolGraphDGStratGraphStateBool, filterFunc))
DGStrat.makeFilter = _DGStrat_makeFilter  # type: ignore

_DGStrat_makeExecute_orig = DGStrat.makeExecute
def _DGStrat_makeExecute(func) -> DGStrat:
	return _DGStrat_makeExecute_orig(_funcWrap(libpymod._Func_VoidDGStratGraphState, func))
DGStrat.makeExecute = _DGStrat_makeExecute  # type: ignore

_DGStrat_makeLeftPredicate_orig = DGStrat.makeLeftPredicate
def _DGStrat_makeLeftPredicate(pred, strat) -> DGStrat:
	return _DGStrat_makeLeftPredicate_orig(_funcWrap(libpymod._Func_BoolDerivation, pred), strat)
DGStrat.makeLeftPredicate = _DGStrat_makeLeftPredicate  # type: ignore

_DGStrat_makeRightPredicate_orig = DGStrat.makeRightPredicate
def _DGStrat_makeRightPredicate(pred, strat) -> DGStrat:
	return _DGStrat_makeRightPredicate_orig(_funcWrap(libpymod._Func_BoolDerivation, pred), strat)
DGStrat.makeRightPredicate = _DGStrat_makeRightPredicate  # type: ignore


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


class _DGStrat_sequenceProxy:
	def __init__(self, strats):
		self.strats = strats

	def  __rshift__(self, other):
		return _DGStrat_sequence__rshift__(self, other)

DGStrat.__rshift__ = _DGStrat_sequence__rshift__  # type: ignore
DGStrat.__rrshift__ = lambda self, other: _DGStrat_sequence__rshift__(other, self)  # type: ignore
Rule.__rshift__ = _DGStrat_sequence__rshift__  # type: ignore


###########################################################
# Graph
###########################################################

inputGraphs = []

_Graph_print_orig = Graph.print
def _Graph_print(self, first=None, second=None) -> Tuple[str, str]:
	if first is None:
		return _Graph_print_orig(self)
	if second is None:
		second = first
	return _Graph_print_orig(self, first, second)
Graph.print = _Graph_print  # type: ignore

_Graph_aut = Graph.aut
Graph.aut = lambda self, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_aut(self, labelSettings)  # type: ignore

_Graph_isomorphism = Graph.isomorphism
Graph.isomorphism = lambda self, g, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_isomorphism(self, g, maxNumMatches, labelSettings)  # type: ignore
_Graph_monomorphism = Graph.monomorphism
Graph.monomorphism = lambda self, g, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Graph_monomorphism(self, g, maxNumMatches, labelSettings)  # type: ignore

_Graph_getGMLString = Graph.getGMLString
Graph.getGMLString = lambda self, withCoords=False: _Graph_getGMLString(self, withCoords)  # type: ignore
_Graph_printGML = Graph.printGML
Graph.printGML = lambda self, withCoords=False: _Graph_printGML(self, withCoords)  # type: ignore

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

Graph.__repr__ = lambda self: str(self) + "(" + str(self.id) + ")"  # type: ignore
Graph.__eq__ = lambda self, other: self.id == other.id  # type: ignore
Graph.__lt__ = lambda self, other: self.id < other.id  # type: ignore
Graph.__hash__ = lambda self: self.id  # type: ignore

def _Graph__setattr__(self, name, value) -> None:
	if name == "image":
		object.__setattr__(self, "image", _funcWrap(libpymod._Func_String, value))
	else:
		_NoNew__setattr__(self, name, value)
Graph.__setattr__ = _Graph__setattr__  # type: ignore


###########################################################
# Rule
###########################################################

inputRules = []

_Rule_print_orig = Rule.print
def _Rule_print(self, first=None, second=None) -> Tuple[str, str]:
	if first is None:
		return _Rule_print_orig(self)
	if second is None:
		second = first
	return _Rule_print_orig(self, first, second)
Rule.print = _Rule_print  # type: ignore

_Rule_isomorphism = Rule.isomorphism
Rule.isomorphism = lambda self, r, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Rule_isomorphism(self, r, maxNumMatches, labelSettings)  # type: ignore
_Rule_monomorphism = Rule.monomorphism
Rule.monomorphism = lambda self, r, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism): _Rule_monomorphism(self, r, maxNumMatches, labelSettings)  # type: ignore

_Rule_getGMLString = Rule.getGMLString
Rule.getGMLString = lambda self, withCoords=False: _Rule_getGMLString(self, withCoords)  # type: ignore
_Rule_printGML = Rule.printGML
Rule.printGML = lambda self, withCoords=False: _Rule_printGML(self, withCoords)  # type: ignore

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

Rule.__repr__ = lambda self: str(self) + "(" + str(self.id) + ")"  # type: ignore
Rule.__eq__ = lambda self, other: self.id == other.id  # type: ignore
Rule.__lt__ = lambda self, other: self.id < other.id  # type: ignore
Rule.__hash__ = lambda self: self.id  # type: ignore


#----------------------------------------------------------
# Composition
#----------------------------------------------------------

def _RCEvaluator__getattribute__(self, name):
	if name == "ruleDatabase":
		return _unwrap(self._ruleDatabase)
	elif name == "products":
		return _unwrap(self._products)
	else:
		return object.__getattribute__(self, name)
RCEvaluator.__getattribute__ = _RCEvaluator__getattribute__  # type: ignore

_RCEvaluator_eval = RCEvaluator.eval
RCEvaluator.eval = lambda self, exp, *, verbosity=2: _unwrap(_RCEvaluator_eval(self, exp, verbosity))  # type: ignore

def rcEvaluator(rules, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism)):
	return libpymod.rcEvaluator(_wrap(libpymod._VecRule, rules), labelSettings)


#----------------------------------------------------------
# RCExp prettification
#----------------------------------------------------------

def rcExp(e) -> RCExpExp:
	if isinstance(e, RCExpExp) or isinstance(e, Rule) or isinstance(e, RCExpUnion):
		return e
	elif isinstance(e, RCExpBind) or isinstance(e, RCExpId) or isinstance(e, RCExpUnbind):
		return e
	elif isinstance(e, RCExpComposeCommon) or isinstance(e, RCExpComposeParallel) or isinstance(e, RCExpComposeSub) or isinstance(e, RCExpComposeSuper):
		return e
	elif isinstance(e, collections.Iterable):
		return RCExpUnion(_wrap(libpymod._VecRCExpExp, [rcExp(a) for a in e]))
	else:
		raise TypeError("Can not convert type '" + str(type(e)) + "' to RCExpExp")

def _rcConvertGraph(g, cls, f) -> RCExpExp:
	if isinstance(g, Graph):
		return cls(g)
	elif isinstance(g, collections.Iterable):
		l = [f(a) for a in g]
		return rcExp(l)
	else:
		raise TypeError("Can not convert type '" + str(type(g)) + "' to " + str(cls))


def rcBind(g) -> RCExpExp:
	return _rcConvertGraph(g, RCExpBind, rcBind)


def rcId(g) -> RCExpExp:
	return _rcConvertGraph(g, RCExpId, rcId)


def rcUnbind(g) -> RCExpExp:
	return _rcConvertGraph(g, RCExpUnbind, rcUnbind)


class _RCCommonOpFirstBound:
	def __init__(self, discardNonchemical, maximum, connected, first) -> None:
		self.discardNonchemical = discardNonchemical
		self.maximum = maximum
		self.connected = connected
		self.first = first

	def __mul__(self, second) -> RCExpExp:
		return RCExpComposeCommon(rcExp(self.first), rcExp(second), self.discardNonchemical, self.maximum, self.connected)


class _RCCommonOpArgsBound:
	def __init__(self, discardNonchemical, maximum, connected) -> None:
		self.discardNonchemical = discardNonchemical
		self.maximum = maximum
		self.connected = connected

	def __rmul__(self, first) -> _RCCommonOpFirstBound:
		return _RCCommonOpFirstBound(self.discardNonchemical, self.maximum, self.connected, first)


class _RCCommonOp:
	def __call__(self, discardNonchemical=True, maximum=False, connected=True):
		return _RCCommonOpArgsBound(discardNonchemical, maximum, connected)

	def __rmul__(self, first) -> RCExpExp:
		return first * self()


rcCommon = _RCCommonOp()


class _RCParallelOpFirstBound:
	def __init__(self, discardNonchemical, first) -> None:
		self.discardNonchemical = discardNonchemical
		self.first = first

	def __mul__(self, second) -> RCExpExp:
		return RCExpComposeParallel(rcExp(self.first), rcExp(second), self.discardNonchemical)


class _RCParallelOpArgsBound:
	def __init__(self, discardNonchemical) -> None:
		self.discardNonchemical = discardNonchemical

	def __rmul__(self, first) -> _RCParallelOpFirstBound:
		return _RCParallelOpFirstBound(self.discardNonchemical, first)


class _RCParallelOp:
	def __call__(self, discardNonchemical=True):
		return _RCParallelOpArgsBound(discardNonchemical)

	def __rmul__(self, first) -> RCExpExp:
		return first * self()


rcParallel = _RCParallelOp()


class _RCSubOpFirstBound:
	def __init__(self, discardNonchemical, allowPartial, first) -> None:
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.first = first

	def __mul__(self, second) -> RCExpExp:
		return RCExpComposeSub(rcExp(self.first), rcExp(second), self.discardNonchemical, self.allowPartial)


class _RCSubOpArgsBound:
	def __init__(self, discardNonchemical, allowPartial) -> None:
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial

	def __rmul__(self, first) -> _RCSubOpFirstBound:
		return _RCSubOpFirstBound(self.discardNonchemical, self.allowPartial, first)


class _RCSubOp:
	def __call__(self, discardNonchemical=True, allowPartial=True):
		return _RCSubOpArgsBound(discardNonchemical, allowPartial)

	def __rmul__(self, first) -> RCExpExp:
		return first * self()


rcSub = _RCSubOp()


class _RCSuperOpFirstBound:
	def __init__(self, discardNonchemical, allowPartial, enforceConstraints, first) -> None:
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.enforceConstraints = enforceConstraints
		self.first = first

	def __mul__(self, second) -> RCExpExp:
		return RCExpComposeSuper(rcExp(self.first), rcExp(second), self.discardNonchemical, self.allowPartial, self.enforceConstraints)


class _RCSuperOpArgsBound:
	def __init__(self, discardNonchemical, allowPartial, enforceConstraints) -> None:
		self.discardNonchemical = discardNonchemical
		self.allowPartial = allowPartial
		self.enforceConstraints = enforceConstraints

	def __rmul__(self, first) -> _RCSuperOpFirstBound:
		return _RCSuperOpFirstBound(self.discardNonchemical, self.allowPartial, self.enforceConstraints, first)


class _RCSuperOp:
	def __call__(self, discardNonchemical=True, allowPartial=True, enforceConstraints=False):
		return _RCSuperOpArgsBound(discardNonchemical, allowPartial, enforceConstraints)

	def __rmul__(self, first) -> RCExpExp:
		return first * self()


rcSuper = _RCSuperOp()


# ----------------------------------------------------------
# Util
# ----------------------------------------------------------

def showDump(f):
	return libpymod.showDump(prefixFilename(f))
