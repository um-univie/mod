from mod import *
from mod.epim.transform.util import *
from mod.epim.encode import graph_terms as term

def biop_str(op, t):
    return """rule [
        ruleID "normalize par"
        left [
            node [ id 1 label "%s"]
            node [ id 2 label "%s"]
            node [ id 3 label "%s"]
            
            edge [source 0 target 1 label "-"]
            edge [source 1 target 2 label "-"]
            edge [source 1 target 3 label "-"]
        ]
        context [
            node [ id 0 label "%s"]
        ]
        right [
            node [ id 2 label "merge(%s)"]
            node [ id 3 label "merge(%s)"]

            edge [source 0 target 2 label "d"]
            edge [source 0 target 3 label "d"]
        ]
    ]""" % (op,t,t,t,t,t)

par_rule = ruleGMLString(biop_str(term.op("par"), term.p()))
sum_rule = ruleGMLString(biop_str(term.op("sum"), term.s()))

def normalform(graph):
    dg = DG(graphDatabase=[graph],
            labelSettings=LabelSettings(LabelType.Term, LabelRelation.Unification))

    with dg.build() as b:
        normal_rules = [par_rule, sum_rule]
        normal_graph = apply_confluent_gts(b, [graph], normal_rules)
    return normal_graph[0], dg
