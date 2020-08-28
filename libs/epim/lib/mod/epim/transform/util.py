from mod import *

def get_go_graphs(ds):
    go_graphs = []
    for d in ds:
        go_graphs.extend([h.graph for h in d.targets if h.graph.vLabelCount("go") > 0])
    return go_graphs

merge_coalesce_rules = [ruleGMLString("""rule [
    ruleID "merge coalesce"
    left [
        edge [ source 0 target 1 label "_X" ]
    ]
    context [
        node [ id 0 label "merge(_lbl0)"]
        node [ id 1 label "_Y"]
        node [ id 2 label "_lbl2"]

        edge [ source 0 target 2 label "d" ]
    ]
    right [
        edge [ source 2 target 1 label "_X" ]
    ]
]""")
, ruleGMLString("""rule [
    ruleID "merge coalesce"
    left [
        edge [ source 0 target 1 label "arg" ]
        edge [ source 2 target 1 label "sync" ]
    ]
    context [
        node [ id 0 label "merge(_lbl0)"]
        node [ id 1 label "_Y"]
        node [ id 2 label "_lbl2"]

        edge [ source 0 target 2 label "d" ]
    ]
    right [
        edge [ source 2 target 1 label "arg-sync" ]
    ]
]""")
, ruleGMLString("""rule [
    ruleID "merge coalesce"
    left [
        edge [ source 0 target 1 label "sync" ]
        edge [ source 2 target 1 label "arg" ]
    ]
    context [
        node [ id 0 label "merge(_lbl0)"]
        node [ id 1 label "_Y"]
        node [ id 2 label "_lbl2"]

        edge [ source 0 target 2 label "d" ]
    ]
    right [
        edge [ source 2 target 1 label "arg-sync" ]
    ]
]""")
, ruleGMLString("""rule [
    ruleID "merge coalesce"
    left [
        edge [ source 0 target 1 label "_X" ]
        edge [ source 2 target 1 label "_X" ]
    ]
    context [
        node [ id 0 label "merge(_lbl0)"]
        node [ id 1 label "_Y"]
        node [ id 2 label "_lbl2"]

        edge [ source 0 target 2 label "d" ]
    ]
    right [
        edge [ source 2 target 1 label "_X" ]
    ]
]""")
]

merge_delete_rule = ruleGMLString("""rule [
    ruleID "merge delete"
    left [
        node [ id 1 label "merge(_lbl2)"]
        edge [source 0 target 1 label "d"]
    ]
    context [
        node [ id 0 label "_lbl1"]
    ]
    right [
    ]
]""")

def merge(b, graph):
    merge_rules = list(merge_coalesce_rules)
    merge_rules.append(merge_delete_rule)
    graph = apply_confluent_gts(b, graph, merge_rules, apply_cleanup_rules = False)
    return graph

def apply_confluent_gts(b, graph, rule_list, apply_cleanup_rules = True):
    oldLimit = config.dg.applyLimit
    config.dg.applyLimit = 1
    for rule in rule_list:
        tmp = b.apply(graph, rule)
        while len(tmp) > 0:
            graph = get_go_graphs(tmp)
            if apply_cleanup_rules:
                graph = merge(b, graph)
            tmp = b.apply(graph, rule)
    config.dg.applyLimit = oldLimit
    return graph