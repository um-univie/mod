from .mod_ import *
import collections
import math
import sys

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
	if hasattr(self, name):
		object.__setattr__(self, name, value)
	else:
		msg = "Can not create new attribute '%s' on object '%s' of type '%s'." % (name, self, type(self))
		msg += "\ndir(" + str(self) + "):\n"
		for name in dir(self): msg += "\t" + name + "\n"
		raise AttributeError(msg)

#----------------------------------------------------------
# Wrappers
#----------------------------------------------------------

def _wrap(C, l):
	lcpp = C()
	lcpp.extend(a for a in l)
	return lcpp
def _unwrap(lcpp):
	l = []
	l.extend(a for a in lcpp)
	return l

def _funcWrap(F, f, resultWrap=None, module=mod_):
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
				import inspect
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

#----------------------------------------------------------
# Chem
#----------------------------------------------------------

BondType.__str__ = mod_._bondTypeToString

#----------------------------------------------------------
# Config
#----------------------------------------------------------

Config.__setattr__ = _NoNew__setattr__
for a in Config.classNames:
	getattr(Config, a).__setattr__ = _NoNew__setattr__
config = getConfig()

#----------------------------------------------------------
# Derivation
#----------------------------------------------------------

Derivation.__repr__ = Derivation.__str__

def _Derivation__getattribute__(self, name):
	if name == "left" or name == "right":
		return object.__getattribute__(self, "_" + name)
	else:
		return object.__getattribute__(self, name)
Derivation.__getattribute__ = _Derivation__getattribute__

def _Derivation__setattr__(self, name, value):
	if name == "left":
		self._left = _wrap(VecGraph, value)
	elif name == "right":
		self._right = _wrap(VecGraph, value)
	else:
		_NoNew__setattr__(self, name, value)
Derivation.__setattr__ = _Derivation__setattr__

def _Derivation__hash__(self):
	assert False
Derivation.__hash__ = _Derivation__hash__

#----------------------------------------------------------
# DG
#----------------------------------------------------------

def dgDerivations(ders):
	return mod_.dgDerivations(_wrap(VecDerivation, ders))
def dgRuleComp(graphs, strat):
	return mod_.dgRuleComp(_wrap(VecGraph, graphs), dgStrat(strat))
def dgDump(graphs, rules, f):
	return mod_.dgDump(_wrap(VecGraph, graphs), _wrap(VecRule, rules), f)

_DG_print_orig = DG.print
def _DG_print(self, printer=None, data=None):
	if printer is None: printer = DGPrinter()
	if data is None: data = DGPrintData(self)
	_DG_print_orig(self, data, printer)
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

def _DG_vertexGraphs(self):
	print("Warning: DG.vertexGraphs is deprecated, use the graph property on DGVertex with DG.vertices instead.")
	for v in self.vertices:
		yield v.graph

def _DG__getattribute__(self, name):
	if name == "derivations":
		print("Warning: DG.derivations is deprecated, use DG.edges instead.")
		print("Note: Returning DG.edges.")
		return self.edges
	elif name == "graphDatabase":
		return _unwrap(self._graphDatabase)
	elif name == "vertexGraphs":
		return _DG_vertexGraphs(self)
	elif name == "products":
		return _unwrap(self._products)
	elif name == "printOptions":
		if hasattr(self, "_cachedPrintOptions"):
			return self._cachedPrintOptions
		else:
			return object.__getattribute__(self, name)						# TODO:
			self._cachedPrintOptions = object.__getattribute__(self, name)
			return self._cachedPrintOptions
	else:
		return object.__getattribute__(self, name)
DG.__getattribute__ = _DG__getattribute__

def _DG__setattr__(self, name, value):
	if name == "_cachedPrintOptions":
		object.__setattr__(self, name, value)
	else:
		_NoNew__setattr__(self, name, value)
DG.__setattr__ = _DG__setattr__

DG.__eq__ = lambda self, other: self.id == other.id
DG.__hash__ = lambda self: self.id


#----------------------------------------------------------
# DGHyperEdge
#----------------------------------------------------------

_DGHyperEdge_print_orig = DGHyperEdge.print
DGHyperEdge.print = lambda self, printer=GraphPrinter(), matchColour="Melon": _DGHyperEdge_print_orig(self, printer, matchColour)

def _DGHyperEdge__getattribute__(self, name):
	if name == "derivation":
		print("Warning: DGHyperEdge.derivation is deprecated")
		d = Derivation()
		for vSrc in self.sources:
			d.left.append(vSrc.graph)
		for vTar in self.targets:
			d.right.append(vTar.graph)
		if len(self.rules) == 0:
			print("Can not get derivation from hyperedge with no rules")
		if len(self.rules) > 0:
			d.rule = list(self.rules)[0]
		return d
	return object.__getattribute__(self, name)
DGHyperEdge.__getattribute__ = _DGHyperEdge__getattribute__


#----------------------------------------------------------
# DerivationRef
#----------------------------------------------------------

_DerivationRef_print_orig = DerivationRef.print
DerivationRef.print = lambda self, printer=GraphPrinter(), matchColour="ForestGreen": _DerivationRef_print_orig(self, printer, matchColour)

DerivationRef.__repr__ = DerivationRef.__str__
def _DerivationRef__hash__(self):
	if not self.isValid(): return hash((0, -1))
	else: return hash((self.id, self.dg.id))
DerivationRef.__hash__ = _DerivationRef__hash__


#----------------------------------------------------------
# DGPrinter
#----------------------------------------------------------

DGPrinter.__setattr__ = _NoNew__setattr__

_DGPrinter_pushVertexVisible_orig = DGPrinter.pushVertexVisible
def _DGPrinter_pushVertexVisible(self, f):
	_DGPrinter_pushVertexVisible_orig(self, _funcWrap(Func_BoolGraphDG, f))
DGPrinter.pushVertexVisible = _DGPrinter_pushVertexVisible

_DGPrinter_pushEdgeVisible_orig = DGPrinter.pushEdgeVisible
def _DGPrinter_pushEdgeVisible(self, f):
	_DGPrinter_pushEdgeVisible_orig(self, _funcWrap(Func_BoolDerivationRef, f))
DGPrinter.pushEdgeVisible = _DGPrinter_pushEdgeVisible

_DGPrinter_pushVertexLabel_orig = DGPrinter.pushVertexLabel
def _DGPrinter_pushVertexLabel(self, f):
	_DGPrinter_pushVertexLabel_orig(self, _funcWrap(Func_StringGraphDG, f))
DGPrinter.pushVertexLabel = _DGPrinter_pushVertexLabel

_DGPrinter_pushEdgeLabel_orig = DGPrinter.pushEdgeLabel
def _DGPrinter_pushEdgeLabel(self, f):
	_DGPrinter_pushEdgeLabel_orig(self, _funcWrap(Func_StringDerivationRef, f))
DGPrinter.pushEdgeLabel = _DGPrinter_pushEdgeLabel

_DGPrinter_pushVertexColour_orig = DGPrinter.pushVertexColour
def _DGPrinter_pushVertexColour(self, f, extendToEdges=True):
	_DGPrinter_pushVertexColour_orig(self, _funcWrap(Func_StringGraphDG, f), extendToEdges)
DGPrinter.pushVertexColour = _DGPrinter_pushVertexColour

_DGPrinter_pushEdgeColour_orig = DGPrinter.pushEdgeColour
def _DGPrinter_pushEdgeColour(self, f):
	_DGPrinter_pushEdgeColour_orig(self, _funcWrap(Func_StringDerivationRef, f))
DGPrinter.pushEdgeColour = _DGPrinter_pushEdgeColour

#----------------------------------------------------------
# DGStrat
#----------------------------------------------------------

def _DGStratGraphState__getattribute__(self, name):
	if name == "subset":
		return _unwrap(self._subset)
	elif name == "universe":
		return _unwrap(self._universe)
	elif name == "derivationRefs":
		return _unwrap(self._derivationRefs)
	else:
		return object.__getattribute__(self, name)
DGStratGraphState.__getattribute__ = _DGStratGraphState__getattribute__

DGStratGraphState.__setattr__ = _NoNew__setattr__

_DGStrat_makeAddStatic_orig = DGStrat.makeAddStatic
def _DGStrat_makeAddStatic(onlyUniverse, graphs):
	return _DGStrat_makeAddStatic_orig(onlyUniverse, _wrap(VecGraph, graphs))
DGStrat.makeAddStatic = _DGStrat_makeAddStatic

_DGStrat_makeAddDynamic_orig = DGStrat.makeAddDynamic
def _DGStrat_makeAddDynamic(onlyUniverse, generator):
	return _DGStrat_makeAddDynamic_orig(onlyUniverse, _funcWrap(Func_VecGraph, generator, resultWrap=VecGraph))
DGStrat.makeAddDynamic = _DGStrat_makeAddDynamic

_DGStrat_makeExecute_orig = DGStrat.makeExecute
def _DGStrat_makeExecute(func):
	return _DGStrat_makeExecute_orig(_funcWrap(Func_VoidDGStratGraphState, func))
DGStrat.makeExecute = _DGStrat_makeExecute

_DGStrat_makeFilter_orig = DGStrat.makeFilter
def _DGStrat_makeFilter(alsoUniverse, filterFunc):
	return _DGStrat_makeFilter_orig(alsoUniverse, _funcWrap(Func_BoolGraphDGStratGraphStateBool, filterFunc))
DGStrat.makeFilter = _DGStrat_makeFilter

_DGStrat_makeLeftPredicate_orig = DGStrat.makeLeftPredicate
def _DGStrat_makeLeftPredicate(pred, strat):
	return _DGStrat_makeLeftPredicate_orig(_funcWrap(Func_BoolDerivation, pred), strat)
DGStrat.makeLeftPredicate = _DGStrat_makeLeftPredicate

_DGStrat_makeParallel_orig = DGStrat.makeParallel
def _DGStrat_makeParallel(l):
	return _DGStrat_makeParallel_orig(_wrap(VecDGStrat, l))
DGStrat.makeParallel = _DGStrat_makeParallel

_DGStrat_makeRightPredicate_orig = DGStrat.makeRightPredicate
def _DGStrat_makeRightPredicate(pred, strat):
	return _DGStrat_makeRightPredicate_orig(_funcWrap(Func_BoolDerivation, pred), strat)
DGStrat.makeRightPredicate = _DGStrat_makeRightPredicate

_DGStrat_makeSequence_orig = DGStrat.makeSequence
def _DGStrat_makeSequence(l):
	return _DGStrat_makeSequence_orig(_wrap(VecDGStrat, l))
DGStrat.makeSequence = _DGStrat_makeSequence

_DGStrat_makeSort_orig = DGStrat.makeSort
def _DGStrat_makeSort(doUniverse, less):
	return _DGStrat_makeSort_orig(doUniverse, _funcWrap(Func_BoolGraphGraphDGStratGraphState, less))
DGStrat.makeSort = _DGStrat_makeSort


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

_Graph_isomorphism = Graph.isomorphism
Graph.isomorphism = lambda self, g, maxNumMatches=1: _Graph_isomorphism(self, g, maxNumMatches)
_Graph_monomorphism = Graph.monomorphism
Graph.monomorphism = lambda self, g, maxNumMatches=1: _Graph_monomorphism(self, g, maxNumMatches)

_Graph_getGMLString = Graph.getGMLString
Graph.getGMLString = lambda self, withCoords=False: _Graph_getGMLString(self, withCoords)
_Graph_printGML = Graph.printGML
Graph.printGML = lambda self, withCoords=False: _Graph_printGML(self, withCoords)

def graphGMLString(d, name=None):
	a = mod_.graphGMLString(d)
	inputGraphs.append(a)
	if name != None:
		a.name = name
	return a
def graphGML(f, name=None):
	a = mod_.graphGML(f)
	inputGraphs.append(a)
	if name != None:
		a.name = name
	return a
def graphDFS(s, name=None):
	a = mod_.graphDFS(s)
	inputGraphs.append(a)
	if name != None:
		a.name = name
	return a
def smiles(s, name=None):
	a = mod_.smiles(s)
	inputGraphs.append(a)
	if name != None:
		a.name = name
	return a

def _Graph__repr__(self):
	return str(self) + "(" + str(self.id) + ")"
Graph.__repr__ = _Graph__repr__

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
# GraphPrinter
#----------------------------------------------------------

GraphPrinter.__setattr__ = _NoNew__setattr__

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

RCEvaluator.__setattr__ = _NoNew__setattr__

_RCEvaluator_eval = RCEvaluator.eval
RCEvaluator.eval = lambda self, e: _unwrap(_RCEvaluator_eval(self, e))

def rcEvaluator(rules):
	return mod_.rcEvaluator(_wrap(VecRule, rules))

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
Rule.isomorphism = lambda self, r, maxNumMatches=1: _Rule_isomorphism(self, r, maxNumMatches)
_Rule_monomorphism = Rule.monomorphism
Rule.monomorphism = lambda self, r, maxNumMatches=1: _Rule_monomorphism(self, r, maxNumMatches)

_Rule_getGMLString = Rule.getGMLString
Rule.getGMLString = lambda self, withCoords=False: _Rule_getGMLString(self, withCoords)
_Rule_printGML = Rule.printGML
Rule.printGML = lambda self, withCoords=False: _Rule_printGML(self, withCoords)

def ruleGMLString(s, invert=False):
	a = mod_.ruleGMLString(s, invert)
	inputRules.append(a)
	return a
def ruleGML(s, invert=False):
	a = mod_.ruleGML(s, invert)
	inputRules.append(a)
	return a

Rule.__repr__ = lambda self: str(self) + "(" + str(self.id) + ")"
Rule.__eq__ = lambda self, other: self.id == other.id
Rule.__hash__ = lambda self: self.id

Rule.__setattr__ = _NoNew__setattr__

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
		raise TypeError("Can not convert type '" + str(type(s)) + "' to DGStrat")

# add
#----------------------------------------------------------

def _DGStrat_add(doUniverse, g, gs):
	if hasattr(g, "__call__"): # assume the dynamic version is meant
		if len(gs) > 0:
			raise TypeError("The dynamic version of addSubset/addUniverse takes exactly 1 argument (" + str(len(gs)) + " given)")
		return DGStrat.makeAddDynamic(doUniverse, g)
	else: # assume the static version was meant
		graphs=[]
		def convertGraphs(graphs, g):
			if isinstance(g, Graph):
				graphs.append(g)
			else:
				graphs.extend(a for a in g)
		convertGraphs(graphs, g)
		for a in gs:
			convertGraphs(graphs, a)
		return DGStrat.makeAddStatic(doUniverse, graphs)
def addUniverse(g, *gs):
	return _DGStrat_add(True, g, gs)
def addSubset(g, *gs):
	return _DGStrat_add(False, g, gs)

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
		return self[2**32-1](strat)
		
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

# sort
#----------------------------------------------------------

class _DGStrat_SortProxy(object):
	def __init__(self, doUniverse):
		self.doUniverse = doUniverse
	def __call__(self, less):
		return DGStrat.makeSort(self.doUniverse, less)

sortSubset = _DGStrat_SortProxy(False)
sortUniverse = _DGStrat_SortProxy(True)

# take
#----------------------------------------------------------

class _DGStrat_TakeProxy(object):
	def __init__(self, doUniverse):
		self.doUniverse = doUniverse
	def __call__(self, amount):
		return DGStrat.makeTake(self.doUniverse, amount)

takeSubset = _DGStrat_TakeProxy(False)
takeUniverse = _DGStrat_TakeProxy(True)

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











