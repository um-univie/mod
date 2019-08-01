mgu("f(_X, a)", "f(_X, _X)")
mgu("f(_X, _Y, _Y)", "f(_X, _X, a)")
mgu("f(_X, g(_X))", "f(_Y, _Y)")
mgu("_x", "f(g(h(a, _b, c), i(d, _e)), j(k(_l, m), n))")
