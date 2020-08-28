from .graph_interface import GraphInterface
from . import graph_terms as term

def ccall(name, args):
    g = GraphInterface()
    vp = g.add_vertex("t(p)", is_input = True)
    vcall = g.add_vertex("t(call(%s))" % name)
    varg_list = {a: g.add_vertex(("v(%s)" % a), is_output = True) for a in set(args)}
    vptr_list = [g.add_vertex("ptr") for a in args]

    g.add_edge(vp, vcall)
    for i, (a, vptr) in enumerate(zip(args, vptr_list)):
        varg = varg_list[a]
        g.add_edge(vcall, vptr, label = str(i))
        g.add_edge(vptr, varg, label = "-")
    return g

def op(op_name, x, y):
    g = GraphInterface()
    vs = g.add_vertex(term.s(), is_input = True)
    # vs = g.add_vertex("p", is_input = True)
    vout = g.add_vertex(term.op(op_name))
    vx = g.add_vertex(term.name(x), is_output = True)
    vy = g.add_vertex(term.name(y), is_output = True)
    vp = g.add_vertex(term.p(), is_output = True)

    # vyd = g.add_vertex("D")
    # vxd = g.add_vertex("D")
    g.add_edge(vs, vout)
    g.add_edge(vout, vp)
    g.add_edge(vout, vx, label = "sync")
    g.add_edge(vout, vy, label = "arg")
    # g.add_edge(vout, vxd)
    # g.add_edge(vout, vyd)
    # g.add_edge(vxd, vx)
    # g.add_edge(vyd, vy)
    return g

def c():
    g = GraphInterface()
    vp = g.add_vertex(term.p(), is_input = True)
    vs = g.add_vertex(term.s(), is_output = True)
    g.add_edge(vp, vs)
    return g

def id(fname, *names):
    g = GraphInterface()
    g.add_vertex(term.name(fname), is_input = True, is_output = True)
    for name in names:
        g = g | id(name)
    return g

def new(*names):
    g = GraphInterface()
    # g.add_vertex("v(" + name + ")", is_input = False, is_output = True)
    for name in names:
        g.add_vertex(term.name(name), is_input = False, is_output = True)
        # g = g | new(name)
    return g

def null(name):
    g = GraphInterface()
    g.add_vertex(term.op(name), is_input = True)
    return g

def null_var(name):
    g = GraphInterface()
    g.add_vertex("v(" + name + ")", is_input = True)
    return g

def biop(opname, vtype):
    g = GraphInterface()
    vtype = "t(" + vtype + ")"
    vp = g.add_vertex(vtype, is_input = True)
    vp1 = g.add_vertex(vtype, is_output = True)
    vp2 = g.add_vertex(vtype, is_output = True)
    vpar = g.add_vertex("t(" + opname + ")")

    g.add_edge(vp, vpar)
    g.add_edge(vpar, vp1)
    g.add_edge(vpar, vp2)
    return g

def restrict(x):
    g = GraphInterface()
    vs = g.add_vertex("p", is_input = True)
    vout = g.add_vertex("restrict")
    vx = g.add_vertex(x, is_output = True)
    vp = g.add_vertex("p", is_output = True)
    g.add_edge(vs, vout)
    g.add_edge(vout, vp)
    g.add_edge(vout, vx)
    return g
