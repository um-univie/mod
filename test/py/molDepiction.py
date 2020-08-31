postSection("Hydrogen moving and charges")
for b in ["", "=", "#"]:
    for c in ["", "+", ".", "+."]:
        for l in [6, 8, 10, 12]:
            for h in ["", "2"]:
                unit = "[CH%s%s]" % (h, c)
                s = [unit + "1"]
                for i in range(l - 1):
                    s.append(unit)
                s.append("1")
                smiles(b.join(s), name="b%s-c%s-l%d-h%s" % (b, c, l, h))
p = GraphPrinter()
p.setMolDefault()
p.simpleCarbons = False
p.withIndex = True
for a in inputGraphs: a.print(p)
inputGraphs[:] = []

postSection("Double/Triple/Lablled bond")
smiles("O=C=C")
smiles("CCCC#N")
smiles("CCC(O)=O")
smiles("CC(=O)C")
graphDFS("C{a}C")
for a in inputGraphs: a.print()
inputGraphs[:] = []
postSection("middle C -> simple C + hidden H")
smiles("CCC")
smiles("CC(O)C")
smiles("N=CC")
for a in inputGraphs: a.print()
inputGraphs[:] = []
postSection("collapse H")
smiles("[H][H]")
graphDFS("[Q][H][R]")
graphDFS("[H]=[R]")
smiles("O")
smiles("CNC")
for a in inputGraphs: a.print()
inputGraphs[:] = []
postSection("Aromatic")
smiles("c1ccccc1")
smiles("c1[nH]c2c(ncnc2n1)N")
for a in inputGraphs: a.print()
inputGraphs[:] = []
postSection("Charges")
smiles("[OH-]")
graphDFS("[O2-]")
smiles("[H+]")
smiles("[NH4+]")
graphDFS("O[N2+](O)(O)O")
graphDFS("[Q]C[Hest]C[Q+]")
smiles("O[C-]C(=O)COP([O-])([O-])=O")
smiles("O[CH-]C(=O)COP([O-])([O-])=O")
for a in inputGraphs: a.print()
