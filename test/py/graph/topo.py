try:
    graphGMLString("""graph [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]""")
    assert False
except InputError as e:
    print(e)
try:
    graphGMLString("""graph [ node [ id 0 label "C" ] node [ id 1 label "C" ] 
            edge [ source 0 target 1 label "-" ] edge [ source 1 target 0 label "-" ]
        ]""")
    assert False
except InputError as e:
    print(e)
try:
    graphGMLString("""graph [ node [ id 0 label "C" ]
            edge [ source 0 target 0 label "-" ]
        ]""")
    assert False
except InputError as e:
    print(e)
