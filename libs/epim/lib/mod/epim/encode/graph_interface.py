import networkx as nx
from networkx.drawing.nx_agraph import to_agraph
import mod
from parse import parse

class GraphInterface:
    def __init__(self):
        self.graph = nx.DiGraph()
        self.label = {}
        self.input = set()
        self.output = set()

    def copy(self):
        c = GraphInterface()
        c.graph = self.graph.copy()
        c.label = dict(self.label)
        c.input = set(self.input)
        c.output = set(self.output)
        return c

    def output_names(self):
        res = {}
        for n in self.output:
            l = self.label[n]
            if l not in res:
                res[l] = [n]
            else:
                res[l].append(n)
            # res[self.label[n]] = n
        return res
    
    def input_names(self):
        res = {}
        for n in self.input:
            l = self.label[n]
            if l not in res:
                res[l] = [n]
            else:
                res[l].append(n)
        return res
    

    def is_input(self, v):
        return v in self.input

    def is_output(self, v):
        return v in self.output

    def add_vertex(self, label, is_input = False, is_output = False):
        idx = len(self.graph.nodes)
        self.graph.add_node(idx)
        self.label[idx] = label
        if is_input: self.input.add(idx)
        if is_output: self.output.add(idx)
        return idx

    def edge_label(self, x, y):
        return self.graph[x][y]["label"]

    def add_edge(self, x, y, label = "-"):
        if self.graph.has_edge(x, y):
            labels = sorted([label, self.edge_label(x, y)])
            self.graph[x][y]["label"] = "-".join(labels)
        else:
            self.graph.add_edge(x, y, label = label)

    def compose_sequential(self, rhs):
        res = rhs.copy()
        lhs = self

        in_name = rhs.input_names()
        idx_map = {}
        for n in lhs.graph.nodes:
            l = lhs.label[n]
            if n in lhs.output and l not in in_name:
                assert(False and "Sequential composition not well defined")
            
            if n not in lhs.output:
                res_n = res.add_vertex(l, lhs.is_input(n), lhs.is_output(n))
                idx_map[n] = res_n
            else:
                idx_map[n] = in_name[l][-1]
                if len(in_name[l]) > 1:
                    in_name[l].pop()

        for x,y in lhs.graph.edges:
            src, tar = idx_map[x], idx_map[y]
            res.add_edge(src, tar, lhs.edge_label(x, y))

        res.input = {idx_map[n] for n in lhs.input}

        return res

    def compose_parallel(self, rhs):
        res = self.copy()
        
        out_name = res.output_names()
        idx_map = {}
        for n in rhs.graph.nodes:
            if rhs.label[n] not in out_name:
                res_n = res.add_vertex(rhs.label[n], rhs.is_input(n), rhs.is_output(n))
                idx_map[n] = res_n
            else:
                idx_map[n] = out_name[rhs.label[n]][0]
                res.input.add(out_name[rhs.label[n]][0])

        for x,y in rhs.graph.edges:
            src, tar = idx_map[x], idx_map[y]
            res.add_edge(src, tar, label = rhs.edge_label(x, y))

        return res

    def draw(self, file_name):
        label = self.label.copy()
        for n in self.input: label[n] += "+"
        for n in self.output: label[n] += "-"
        A = to_agraph(self.graph)
        # print(A.nodes())
        for n in A.nodes_iter():
             n.attr["label"] = label[int(n)]
        # print(A)
        A.layout('dot')
        A.draw(file_name)
        # nx.draw(self.graph, labels = label, with_labels = True)

    def to_modgraph(self):
        gml_string = "graph [\n"
        for n in self.graph.nodes:
            lbl = self.label[n]
            if lbl[0] == "v":
                res = parse("v({},{})", lbl)
                lbl = "v(" + res[0] + ")"
            gml_string += "node [ id %d label \"%s\"]\n" % (n, lbl)
            # if self.label[n][0] != "v" or n in self.output:
            #     gml_string += "node [ id %d label \"%s\"]\n" % (n, self.label[n])
            # else:
            #     gml_string += "node [ id %d label \"%s\"]\n" % (n, "v(A)")
        nid = len(self.graph.nodes)
        gml_string += "node [ id %d label \"%s\"]\n" % (nid, "go")
        gml_string += "edge [source %d target %d label \"-\" ]\n" % (nid, list(self.input)[0])

        nid += 1
        for x, y in self.graph.edges:
            gml_string += "edge [source %d target %d label \"%s\" ]\n" % (x, y, self.edge_label(x, y))
        gml_string += "]"
        print(gml_string)
        return mod.graphGMLString(gml_string)

    def __lshift__(self, other):
        return self.compose_sequential(other)

    def __or__(self, other):
        return self.compose_parallel(other)
