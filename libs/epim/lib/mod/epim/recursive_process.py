from mod import *
from .process import Process
from .encode import graph_terms as term

def make_call_rule(name, args, p):
    g = p.encode()
    arg_offset = 3
    arg_map = {}
    for i, arg in enumerate(args):
        arg_map[term.name(arg)] = i + arg_offset

    vgo = next(v for v in g.vertices if v.stringLabel == "go")
    vgop = next(e.target for e in vgo.incidentEdges)
    vmap = {vgo.id: (0, False), vgop.id: (1, False)}
    vid = arg_offset + len(args)
    for v in g.vertices:
        if v.id in vmap: continue
        if v.stringLabel in arg_map:
            vmap[v.id] = (arg_map[v.stringLabel], False)
        else:
            vmap[v.id] = (vid, True)
            vid += 1

    rule_str = ["rule ["]
    rule_str.append("ruleID \"%s\"" % name)
    rule_str.append("left [")

    rule_str.append("node [ id 2 label \"t(call(%s))\" ]" % name)

    for i, arg in enumerate(args):
        rule_str.append("node [ id %d label \"ptr\" ]" % (i + arg_offset))

    rule_str.append("edge [ source 1 target 2 label \"-\" ]")
    for i, arg in enumerate(args):
        rule_str.append("edge [ source 2 target %d label \"%d\" ]" % (i + arg_offset, i))

    rule_str.append("]\ncontext [")
    rule_str.append("node [ id 0 label \"go\" ]")
    rule_str.append("node [ id 1 label \"t(p)\" ]")

    rule_str.append("edge [ source 0 target 1 label \"-\" ]")
    rule_str.append("]\nright [")
    for v in g.vertices:
        vid, should_create = vmap[v.id]
        if not should_create: continue

        rule_str.append("node [ id %d label \"%s\" ]" % (vid, v.stringLabel))

    for i, arg in enumerate(args):
        rule_str.append("node [ id %d label \"merge(ptr)\" ]" % (i + arg_offset))

    for e in g.edges:
        if {e.source, e.target} == {vgo, vgop}: continue
        src, tar, lbl = vmap[e.source.id][0], vmap[e.target.id][0], e.stringLabel
        # if "ptr" in {e.target.stringLabel, e.source.stringLabel} and e.stringLabel == "-":
        #     lbl = "d"

        rule_str.append("edge [ source %d target %d label \"%s\" ]" % (src, tar, lbl))
    rule_str.append("]\n]")
    rule_str = "\n".join(rule_str)
    print(rule_str)
    return ruleGMLString(rule_str)

class RecursiveProcess:
    def __init__(self):
        self._process = {}
        self._process_rule = {}

    def add(self, name, args, p):
        assert(isinstance(name, str) and isinstance(p, Process))
        assert(isinstance(args, list))
        r = make_call_rule(name, args, p)
        self._process[name] = (args, p)
        self._process_rule[name] = r
        # r.print()

    def rules(self):
        return self._process_rule.values()
