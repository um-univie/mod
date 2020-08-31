
from sphinx.errors import NoUri

anys = [
	"ignoreRuleLabelTypes", "labelSettings",
]
exacts = {
	'cpp': [
		"mod",
		"AtomIds",
		"graph", "graph::GraphLess",
		"rule", "rule::RCExp", "RCExp",
		"dg",
		"lib", "boost",
		"causality",
	],
	'py': [
		"Callable",
		"GraphIncidentEdgeRange", "GraphVertexRange", "GraphEdgeRange",
		"RuleIncidentEdgeRange", "RuleVertexRange", "RuleEdgeRange",
		"RuleLeftGraphIncidentEdgeRange", "RuleLeftGraphVertexRange", "RuleLeftGraphEdgeRange",
		"RuleContextGraphIncidentEdgeRange", "RuleContextGraphVertex", "RuleContextGraphVertexRange", "RuleContextGraphEdgeRange",
		"RuleRightGraphIncidentEdgeRange", "RuleRightGraphVertexRange", "RuleRightGraphEdgeRange",
		"DGRuleRange", "DGSourceRange", "DGTargetRange", "DGOutEdgeRange", "DGInEdgeRange", "DGEdgeRange", "DGVertexRange",
	],
}
exactsType =  {
	('std', 'token'): [
		"atomSymbol", "singleDigit",
		"quoteEscapedString", "bracketEscapedString", "braceEscapedString",
		"identifier", "unsignedInt", "int", "double",
		"graphs", "rule", "rules", "strats", "derivationPred", "executeFunc", "filterPred",
	]
}
prefixes = {
	'cpp': [
		"mod::", "Function<", "lib::", "boost::",
	]
}

def missing_reference(app, env, node, contnode):
	target = node['reftarget']
	typ = node['reftype']
	domain = node.get('refdomain')
	if domain in exacts and target in exacts[domain]:
		raise NoUri()
	if (domain, typ) in exactsType and target in exactsType[(domain, typ)]:
		raise NoUri()
	if domain in prefixes and any(target.startswith(a) for a in prefixes[domain]):
		raise NoUri()
	if not domain and typ == "any" and target in anys:
		raise NoUri()


def setup(app):
	app.connect('missing-reference', missing_reference)
	return {
		'version': '0.1',
		'parallel_read_safe': True,
		'parallel_write_safe': True,
	}
