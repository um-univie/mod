from .encode.graph_constants import *
from .transform.normalform import normalform
from .gen_image import setImage

from parse import parse

class ProcessConfig:
    def __init__(self):
        self.print_args = True

process_config = ProcessConfig()

class Name:
    def __init__(self, name):
        assert(isinstance(name, str))
        self.name = name
        self.nest_num = 0

    def __str__(self):
        return self.name

    def __eq__(self, other):
        assert(isinstance(other, Name))
        return self.name == other.name

    def __ne__(self, other):
        assert(isinstance(other, Name))
        return self.name != other.name

    def __hash__(self):
        return hash(self.name)

_name_id = 0
def get_name(n, nested_num):
    return Name(n.name + "," + str(nested_num))
    # return Name(n.name)

class Call:
    def __init__(self, name, args):
        assert(isinstance(name, str) and isinstance(args, list))
        self.name = name
        self.args = args

    def free_names(self):
        return set(self.args)

    def encode(self, fn):
        args = [get_name(a, fn[a]) for a in self.args]
        fn_names = [get_name(n, fn[n]) for n in fn.keys()]
        # return ccall(self.name, args) | new(*fn.keys())
        return ccall(self.name, args) | new(*fn_names)

    def visit(self, visitor):
        visitor(self)

    def __str__(self):
        if process_config.print_args:
            return "%s(%s)" % (self.name, ", ".join([str(a) for a in self.args]))
        else:
            return "%s()" % (self.name)


class Sum:
    def __init__(self, p1, p2):
        self.p1 = p1
        self.p2 = p2

    def free_names(self):
        return self.p1.free_names() | self.p2.free_names()

    def encode(self, fn):
        return biop("sum", "s") << (self.p1.encode(fn) | self.p2.encode(fn))

    def visit(self, visitor):
        visitor(self)
        self.p1.visit(visitor)
        self.p2.visit(visitor)

    def __str__(self):
        return "%s + %s" % (str(self.p1), str(self.p2))

class Par:
    def __init__(self, P1, P2):
        self.p1 = P1
        self.p2 = P2

    def free_names(self):
        return self.p1.free_names() | self.p2.free_names()

    def encode(self, fn):
        return biop("par", "p") << (self.p1.encode(fn) | self.p2.encode(fn))

    def visit(self, visitor):
        visitor(self)
        self.p1.visit(visitor)
        self.p2.visit(visitor)

    def __str__(self):
        return "%s | %s" % (str(self.p1), str(self.p2))

class Coerce:
    def __init__(self, p = None):
        self.p = p

    def free_names(self):
        return self.p.free_names()

    def encode(self, fn):
        return c() << self.p.encode(fn)

    def visit(self, visitor):
        visitor(self)
        self.p.visit(visitor)

    def __str__(self):
        return str(self.p)


class Out:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.p = Null()

    def free_names(self):
        return {self.x, self.y} | self.p.free_names()

    def encode(self, fn):
        x, y = self.x, self.y
        x = get_name(x, fn[x])
        y = get_name(y, fn[y])
        # g = c() << op("out", x, y) << (self.p.encode(fn) | id(x) | id(y))
        g = op("out", x, y) << (self.p.encode(fn) | id(x) | id(y))
        return g

    def visit(self, visitor):
        visitor(self)
        self.p.visit(visitor)

    def __str__(self):
        return "out(%s, %s).%s" % (self.x, self.y, str(self.p))

class In:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.p = Null()

    def free_names(self):
        return (self.p.free_names() - {self.y}) | {self.x}

    def encode(self, fn):
        # assert(self.y not in fn)

        x, y = self.x, self.y
        fn = fn.copy()
        x = get_name(x, fn[x])
        global _name_id
        _name_id += 1
        fn[y] = _name_id

        y = get_name(y, fn[y])
        
        g = (self.p.encode(fn) | id(x, y))
        # g = (self.p.encode(fn | {x}) | id(x, y))
        # g = op("in", x, y) << g << (null_var(y) | id(*fn))
        g = op("in", x, y) << g
        return g

    def visit(self, visitor):
        visitor(self)
        self.p.visit(visitor)

    def __str__(self):
        return "in(%s, %s).%s" % (self.x, self.y, str(self.p))

class Restrict:
    def __init__(self, y):
        self.y = y
        self.p = Null()

    def free_names(self):
        return self.p.free_names() - {self.y}

    def encode(self, fn):
        y = self.y
        fn = fn.copy()
        global _name_id
        _name_id += 1
        fn[y] = _name_id
        return self.p.encode(fn)

    def visit(self, visitor):
        visitor(self)
        self.p.visit(visitor)

    def __str__(self):
        return "restrict(%s).%s" % (self.y, str(self.p))

class Null:
    def __init__(self):
        self.p = None
        return

    def free_names(self):
        return set()

    def encode(self, fn):
        fn_names = [get_name(n, fn[n])  for n in fn.keys()]
        # return null("p") | new(*fn)
        return null("p") | new(*fn_names)

    def visit(self, visitor):
        visitor(self)

    def __str__(self):
        return "0"

class Process:
    def __init__(self, name = None, args = None):
        null = Null()
        self.head = null
        self.tail = null

        self.name = name
        self.args = args

    def _append(self, exp):
        if isinstance(self.tail, (Call, Sum, Par)):
            raise NameError("Parse Error: Cannot append to end of: " + str(self))

        if isinstance(self.head, Null):
            self.head = exp
            self.tail = exp
        else:
            self.tail.p = exp
            self.tail = exp

    def output(self, x, y):
        assert(isinstance(x, Name) and isinstance(y, Name))
        self._append(Coerce())
        self._append(Out(x, y))
        return self

    def input(self, x, y):
        assert(isinstance(x, Name) and isinstance(y, Name))
        self._append(Coerce())
        self._append(In(x, y))
        return self

    def restrict(self, x):
        assert(isinstance(x, Name))
        self._append(Restrict(x))
        return self

    def call(self, p, args):
        self._append(Call(p, args))
        return self

    def par(self, p1, p2):
        assert(isinstance(p1, Process) and isinstance(p2, Process))

        self._append(Par(p1, p2))
        return self

    def sum(self, p1, p2):
        assert(isinstance(p1, Process) and isinstance(p2, Process))

        self._append(Coerce())
        self._append(Sum(p1, p2))

        if type(p1.head) is Coerce: p1.head = p1.head.p
        if type(p2.head) is Coerce: p2.head = p2.head.p
        return self

    def process(self, p):
        assert(isinstance(p, Process))
        self.tail.p = p.head
        self.tail = p.tail
        return self

    def free_names(self):
        fn = self.head.free_names()
        return fn

    def encode(self, fn = None, use_normalform = True, use_interface = False):
        is_head = (fn is None)
        if fn is None:
            fn = {n: 0 for n in self.free_names()}
        tree = self.head.encode(fn)
        if is_head and not use_interface:
            G = tree.to_modgraph()
            if use_normalform:
                G, _dg = normalform(G)
            setImage(G)
            return G
        else:
            return tree

    def visit(self, visitor):
        visitor(self)
        self.head.visit(visitor)

    def __or__(self, other):
        return Process().par(self, other)

    def __add__(self, other):
        return Process().sum(self, other)

    def __str__(self):
        s = ""
        if self.name is not None:
            s = self.name + "(" + ", ".join(self.args) + ")" + " := "

        s += str(self.head)
        return s

    def from_modgraph_vertex(v, marked, call_p = None):
        num_adj = len([e for e in v.incidentEdges])
        print("lbl:", v.stringLabel, "num_adj:", num_adj)
        if v.stringLabel == "go":
            for e in v.incidentEdges:
                marked[e.target.id] = True
                return Process.from_modgraph_vertex(e.target, marked, call_p)
        elif v.stringLabel == "t(p)" and num_adj == 1:
            p = Process()
            return p
        elif v.stringLabel == "t(p)" and num_adj == 2:
            next_v = next(u.target for u in v.incidentEdges if marked[u.target.id] == False)
            marked[next_v.id] = True
            return Process.from_modgraph_vertex(next_v, marked, call_p)
        elif v.stringLabel == "t(p)" and num_adj > 2:
            ps = []
            for e in v.incidentEdges:
                if marked[e.target.id]: continue
                marked[e.target.id] = True
                ps.append(Process.from_modgraph_vertex(e.target, marked, call_p))
            par_p = ps[0]
            for p in ps[1:]:
                par_p = par_p | p
            return par_p
        elif v.stringLabel == "t(s)" and num_adj == 2:
            next_v = next(u.target for u in v.incidentEdges if marked[u.target.id] == False)
            marked[next_v.id] = True
            return Process.from_modgraph_vertex(next_v, marked, call_p)
        elif v.stringLabel == "t(s)" and num_adj > 2:
            ps = []
            for e in v.incidentEdges:
                if marked[e.target.id]: continue
                marked[e.target.id] = True
                ps.append(Process.from_modgraph_vertex(e.target, marked, call_p))
            sum_p = ps[0]
            for p in ps[1:]:
                sum_p = sum_p + p
            return sum_p
        elif v.stringLabel == "t(out)":
            sync_v = next(e.target for e in v.incidentEdges 
                    if e.stringLabel == "sync" or e.stringLabel == "arg-sync")
            arg_v = next(e.target for e in v.incidentEdges 
                    if e.stringLabel == "arg" or e.stringLabel == "arg-sync")
            marked[sync_v.id] = True
            marked[arg_v.id] = True
            p_v = next(e.target for e in v.incidentEdges
                    if not marked[e.target.id])
            p = Process.from_modgraph_vertex(p_v, marked, call_p)
            x = Name(parse("v({})", sync_v.stringLabel)[0])
            y = Name(parse("v({})", arg_v.stringLabel)[0])
            p = Process().output(x, y).process(p)
            return p
        elif v.stringLabel == "t(in)":
            sync_v = next(e.target for e in v.incidentEdges 
                    if e.stringLabel == "sync" or e.stringLabel == "arg-sync")
            arg_v = next(e.target for e in v.incidentEdges 
                    if e.stringLabel == "arg" or e.stringLabel == "arg-sync")
            marked[sync_v.id] = True
            marked[arg_v.id] = True
            p_v = next(e.target for e in v.incidentEdges 
                    if not marked[e.target.id])
            marked[p_v.id] = True
            print("IM HERE")
            p = Process.from_modgraph_vertex(p_v, marked, call_p)
            print(p)
            x = Name(parse("v({})", sync_v.stringLabel)[0])
            y = Name(parse("v({})", arg_v.stringLabel)[0])
            p = Process().input(x, y).process(p)
            return p
        else:
            # assert(call_p is not None)
            call_name = parse("t(call({}))", v.stringLabel)[0]
            edge_ptr = [(e, int(e.stringLabel)) for e in v.incidentEdges if e.stringLabel != "-"]
            edge_args = []
            for ptr_e, i in edge_ptr:
                e = next(e for e in ptr_e.target.incidentEdges if e.stringLabel == "-")
                edge_args.append((e, i))

            edge_args = sorted(edge_args, key = lambda ei: ei[1])
            # [print(e.source.stringLabel) for e, i in edge_args]
            args = []
            for e, i in edge_args:
                args.append(Name(parse("v({})", e.target.stringLabel)[0]))
            return Process().call(call_name, args)
                
        assert(False)

    def from_modgraph(mg, callable_processes = None):
        root = next(v for v in mg.vertices if v.stringLabel == "go")
        marked = [False for v in mg.vertices]
        marked[root.id] = True
        p = Process.from_modgraph_vertex(root, marked, callable_processes)
        return p

def names(name_str):
    assert(isinstance(name_str, str))
    name_list = name_str.split()
    return [Name(name) for name in name_list]

