def fail(s):
    try:
        a = ruleGMLString("rule [ %s ]" % s)
        a.print()
        print(a.getGMLString())
        assert False
    except InputError as e:
        print(e)

for side in "left", "context", "right":
    fail("""
        %s [
            node [ id 0 label "C" ]
            node [ id 1 label "C" ]
            edge [ source 0 target 1 label "-" ]
            edge [ source 1 target 0 label "-" ]
        ]
    """ % side)
    fail("""
        %s [
            node [ id 0 label "C" ]
            edge [ source 0 target 0 label "-" ]
        ]
    """ % side)
