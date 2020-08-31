class ImageConfig:
    def __init__(self):
        self.use_custom = True
        self.hide_image = False
        self.show_pointers = False
        self.show_variables = True
        self.show_root = False
        self.collapse_two_edges = True

image_config = ImageConfig()

def print_edge(e):
    u, v = e.source, e.target
    if not image_config.show_root and "go" in [u.stringLabel, v.stringLabel]:
        return False

    if not image_config.show_pointers and "ptr" in [u.stringLabel, v.stringLabel]:
        return False

    if not image_config.show_variables and "v" in [u.stringLabel[0], v.stringLabel[0]]:
        return False
    return True

def get_collapsed_v(u, v, marked):
    while v.stringLabel in ["t(p)", "t(s)"] and v.degree == 2:
        e1, e2 = v.incidentEdges
        if e1.target.id == u.id:
            e1 = e2
        marked[v.id] = True
        u, v = v, e1.target
    return v

def setImage(mg, use_raw = False):
    if not image_config.use_custom: return

    fName = "out/graph_%d" % mg.id
    def noImage():
        with open(fName + ".dot", "w") as f:
            f.write("graph {\n")
            f.write("%d [ label=\"%s\"];\n" % (0, mg.id))
            f.write("}\n")
        return fName

    def makeImage():
        with open(fName + ".dot", "w") as f:
            f.write("graph {\n")
            f.write("node [shape = plaintext, fontsize = 10, height=0.2, fixedsize=true];");
            f.write("edge [fontsize = 10, len = 0.1];");
            f.write("ranksep=0.0002;")
            f.write("layout=dot;")

            show = [False for v in mg.vertices]
            for v in mg.vertices:
                if not image_config.show_root and v.stringLabel == "go": continue
                if not image_config.show_pointers and v.stringLabel == "ptr": continue
                if not image_config.show_variables and v.stringLabel[0] == "v": continue

                if image_config.collapse_two_edges and v.stringLabel in ["t(p)", "t(s)"] and v.degree <= 2: continue
                show[v.id] = True
                f.write("%d [ label=\"%s\"];\n" % (v.id, v.stringLabel))

            root = next(v for v in mg.vertices if v.stringLabel == "go")
            marked = [False for v in mg.vertices]
            marked[root.id] = True
            Q = [root]
            while len(Q) > 0:
                v = Q.pop()
                for e in v.incidentEdges:
                    u = e.target
                    if v.id == u.id: u = e.source
                    if marked[u.id]: continue
                    
                    if print_edge(e):
                        e_lbl = e.stringLabel
                        if e_lbl == "-": 
                            e_lbl = ""
                        
                        if image_config.collapse_two_edges:
                            u = get_collapsed_v(v,u, marked)

                        if show[e.source.id] and show[u.id]:
                            print(e.target.stringLabel)
                            print("%d -- %d [ label=\"%s\" ];\n" % (e.source.id, u.id, e_lbl))
                            f.write("%d -- %d [ label=\"%s\" ];\n" % (e.source.id, u.id, e_lbl))

                    if not image_config.show_pointers and v.stringLabel == "ptr" and image_config.show_variables:
                        ptr_src, ptr_tar = [ptr_e.target for ptr_e in v.incidentEdges]
                        ptr_e1, ptr_e2 = [ptr_e for ptr_e in v.incidentEdges]
                        ptr_lbl = ptr_e1.stringLabel
                        if ptr_lbl == "-":
                            ptr_lbl = ptr_e2.stringLabel

                        f.write("%d -- %d [ label=\"%s\" ];\n" % (ptr_src.id, ptr_tar.id, ptr_lbl))

                    if u.stringLabel[0] == "v": continue


                    marked[u.id] = True
                    Q.append(u)
            f.write("}\n")
        return fName

    def makeRawImage():
        with open(fName + ".dot", "w") as f:
            f.write("graph {\n")
            f.write("node [shape = plaintext, fontsize = 10, height=0.2, fixedsize=true];");
            f.write("edge [fontsize = 10, len = 0.1];");
            f.write("ranksep=0.0002;")
            f.write("layout=dot;")

            for v in mg.vertices:
                f.write("%d [ label=\"%s\"];\n" % (v.id, v.stringLabel))

            root = next(v for v in mg.vertices if v.stringLabel == "go")
            marked = [False for v in mg.vertices]
            marked[root.id] = True
            Q = [root]
            used_edges = set()
            while len(Q) > 0:
                v = Q.pop()
                for e in v.incidentEdges:
                    e_lbl = e.stringLabel
                    if e_lbl == "-": e_lbl = ""
                    u = e.target
                    if v.id == u.id: u = e.source
                    if marked[u.id]: continue
                    f.write("%d -- %d [ label=\"%s\" ];\n" % (v.id, u.id, e_lbl))
                    used_edges.add(tuple(sorted((v.id, u.id))))
                    if u.stringLabel[0] == "v": continue
                    marked[u.id] = True
                    Q.append(u)
            for e in mg.edges:
                v, u = e.source, e.target
                e_lbl = e.stringLabel
                if e_lbl == "-": e_lbl = ""
                if tuple(sorted((v.id, u.id))) not in used_edges:
                    f.write("%d -- %d [ label=\"%s\" ];\n" % (v.id, u.id, e_lbl))


            f.write("}\n")
        return fName
            
    if image_config.hide_image:
        mg.image = noImage
    else:
        if use_raw:
            mg.image = makeRawImage
        else:
            mg.image = makeImage
    mg.imageCommand = "gv base \"%s\" pdf" % fName
