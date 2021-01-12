from mod import *
from mod.epim.transform.util import apply_confluent_gts, merge, get_go_graphs
from mod.epim.process import Process
from mod.epim.gen_image import setImage
from mod.epim.recursive_process import RecursiveProcess

reduction_rules =[ ruleGMLString("""rule [
    ruleID "reduction rule"
    left [
        node [ id 5 label "t(p)" ]
        node [ id 9 label "t(p)" ]
        node [ id 6 label "v(_Z)" ]

        node [ id 4 label "t(in)" ]
        node [ id 8 label "t(out)" ]

        node [ id 2 label "t(s)" ]
        node [ id 3 label "t(s)" ]

        edge [ source 1 target 2 label "-" ]
        edge [ source 1 target 3 label "-" ]
        edge [ source 2 target 4 label "-" ]
        edge [ source 4 target 5 label "-" ]
        edge [ source 4 target 6 label "arg" ]
        edge [ source 4 target 7 label "sync" ]
        edge [ source 3 target 8 label "-" ]
        edge [ source 8 target 7 label "sync" ]
        edge [ source 8 target 10 label "arg" ]
        edge [ source 8 target 9 label "-" ]

    ]
    context [
        node [ id 0 label "go" ]
        node [ id 1 label "t(p)" ]
        node [ id 10 label "v(_Y)" ]
        node [ id 7 label "v(_X)" ]

        edge [ source 0 target 1 label "-" ]
    ]
    right [
        node [ id 5 label "merge(t(p))" ]
        node [ id 9 label "merge(t(p))" ]
        node [ id 6 label "merge(v(_Z))" ]

        node [ id 2 label "gc" ]
        node [ id 3 label "gc" ]

        edge [ source 1 target 5 label "d" ]
        edge [ source 1 target 9 label "d" ]
        edge [ source 6 target 10 label "d" ]
    ]
]""")

, ruleGMLString("""rule [
    ruleID "reduction rule"
    left [
        node [ id 5 label "t(p)" ]
        node [ id 9 label "t(p)" ]
        node [ id 6 label "v(_Z)" ]

        node [ id 4 label "t(in)" ]
        node [ id 8 label "t(out)" ]

        node [ id 2 label "t(s)" ]
        node [ id 3 label "t(s)" ]

        edge [ source 1 target 2 label "-" ]
        edge [ source 1 target 3 label "-" ]
        edge [ source 2 target 4 label "-" ]
        edge [ source 4 target 5 label "-" ]
        edge [ source 4 target 6 label "arg" ]
        edge [ source 4 target 7 label "sync" ]
        edge [ source 3 target 8 label "-" ]
        edge [ source 8 target 7 label "arg-sync" ]
        edge [ source 8 target 9 label "-" ]

    ]
    context [
        node [ id 0 label "go" ]
        node [ id 1 label "t(p)" ]
        node [ id 7 label "v(_X)" ]

        edge [ source 0 target 1 label "-" ]
    ]
    right [
        node [ id 5 label "merge(t(p))" ]
        node [ id 9 label "merge(t(p))" ]
        node [ id 6 label "merge(v(_Z))" ]

        node [ id 2 label "gc" ]
        node [ id 3 label "gc" ]

        edge [ source 1 target 5 label "d" ]
        edge [ source 1 target 9 label "d" ]
        edge [ source 6 target 7 label "d" ]
    ]
]""")
]

cleanup_reduction_rules = [
ruleGMLString("""rule [
    left [
        node [ id 1 label "t(_X)" ]

        edge [ source 0 target 1 label "_Y" ]
    ]
    context [
        node [ id 0 label "gc" ]
    ]
    right [
        node [ id 1 label "gc" ]
    ]
]""")
, ruleGMLString("""rule [
    left [
        node [ id 1 label "ptr" ]

        edge [ source 0 target 1 label "_Y" ]
    ]
    context [
        node [ id 0 label "gc" ]
    ]
    right [
        node [ id 1 label "gc" ]
    ]
]""")
, ruleGMLString("""rule [
    left [

        edge [ source 0 target 1 label "_Y" ]
    ]
    context [
        node [ id 0 label "gc" ]
        node [ id 1 label "v(_X)" ]
    ]
    right [
    ]
]""")
]

cleanup_call = [ruleGMLString("""rule [
    left [
        edge [ source 0 target 1 label "-" ]
    ]
    context [
        node [ id 1 label "v(_X)" ]
        node [ id 0 label "merge(ptr)" ]
    ]
    right [
        edge [ source 0 target 1 label "d" ]
    ]
]""")
]

class GraphDerivations:
    def __init__(self, graph, derivations):
        p = Process.from_modgraph(graph)
        self.graph = graphGMLString(graph.getGMLString(), str(p))
        setImage(self.graph)
        self.derivations = derivations

def make_rule(gleft, gright):
    rule_str = ["rule ["]
    rule_str.append("left [")
    for v in gleft.vertices:
        rule_str.append("node [ id %d label \"%s\" ]" % (v.id, v.stringLabel))
    for e in gleft.edges:
        src, tar, lbl = e.source.id, e.target.id, e.stringLabel
        rule_str.append("edge [ source %d target %d label \"%s\" ]" % (src, tar, lbl))
    rule_str.append("]")
    rule_str.append("context []")
    rule_str.append("right [")
    offset = gleft.numVertices
    for v in gright.vertices:
        rule_str.append("node [ id %d label \"%s\" ]" % (v.id + offset, v.stringLabel))
    for e in gright.edges:
        src, tar, lbl = e.source.id + offset, e.target.id + offset, e.stringLabel
        rule_str.append("edge [ source %d target %d label \"%s\" ]" % (src, tar, lbl))
    rule_str.append("]")
    rule_str.append("]")
    return ruleGMLString("\n".join(rule_str))

class ReductionDG:
    def __init__(self, p, rp = RecursiveProcess()):
        self.graph = p.encode()
        self.recursive_process = rp
        self._example_list = []

    def clean_dg(self):
        oldLimit = config.dg.applyLimit
        config.dg.applyLimit = 1
        assert(self.raw_dg is not None)
        dg = DG(graphDatabase=[gd.graph for gd in self.graph_derivations],
                labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification))
        with dg.build() as b:
            for gd in self.graph_derivations:
                graph = gd.graph
                for d in gd.derivations:
                    r = make_rule(graph, d)
                    b.apply([graph], r)
        self.dg = dg
        config.dg.applyLimit = oldLimit

    def _make_example_dg(self):
        graphs = self._example_list[0]
        dg = dgBuild([g for g in graphs],
                labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification))
        print("NUM GRAPHS:", len(graphs))
        with dg.build() as b:
            i = 0
            for g in graphs[1:]:
                print(g.name)
                r = make_rule(graphs[i], g)
                b.apply([graphs[i]], r, limit = 1)
                i += 1
        self.example_dg = dg

    def calc(self, max_iter = 0):
        dg = DG(graphDatabase=[],
                labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification))

        self.graph_derivations = []
        used_graphs = set([self.graph])
        with dg.build() as b:
            current_graph_list = [self.graph]
            count = 0
            while len(current_graph_list) > 0:
                count += 1
                if count == max_iter:
                    break

                new_graph_list = []
                for g in current_graph_list:
                    gd = GraphDerivations(graph = g, derivations = [])
                    for r in reduction_rules:
                        raw_derivations = get_go_graphs(b.apply([g], r))

                        for d in raw_derivations:
                            ex_list = [g, d]
                            d = merge(b, [d])
                            ex_list.append(d[0])
                            d = apply_confluent_gts(b, d, cleanup_reduction_rules,
                                    apply_cleanup_rules = False)
                            ex_list.append(d[0])
                            gd.derivations.extend(d)
                            if d[0] not in used_graphs:
                                used_graphs.add(d[0])
                                new_graph_list.extend(d)

                            self._example_list.append(ex_list)

                    call_rules = self.recursive_process.rules()
                    expanded_calls = apply_confluent_gts(b, [g], call_rules, 
                            apply_cleanup_rules = False)
                    expanded_calls = apply_confluent_gts(b, expanded_calls, cleanup_call)
                    if expanded_calls[0] != g:
                        gd.derivations.extend(expanded_calls)
                        if expanded_calls[0] not in used_graphs:
                            used_graphs.add(expanded_calls[0])
                            new_graph_list.extend(expanded_calls)

                    self.graph_derivations.append(gd)
                current_graph_list = new_graph_list
        self.raw_dg = dg
        self.clean_dg()

    def print(self, use_raw_dg = False, use_example = False):
        dg = self.dg
        if use_raw_dg:
            dgp = DGPrinter()

            for g in self.raw_dg.vertices:
                setImage(g.graph, use_raw = True)
            dgp.pushVertexVisible(lambda g, dg: g.vLabelCount("go") > 0)
            dgp.withShortcutEdgesAfterVisibility = True
            if use_example:
                self._make_example_dg()
                dg = self.example_dg
            else:
                dg = self.raw_dg
            dg.print(dgp)
            return
        dg.print()
