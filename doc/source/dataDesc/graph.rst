.. _graph-tikz:

Tikz (Graph)
############

Graphs are visualised using generated `Tikz`_ code.
The coordinates for the layout is either generated using `Open Babel`_ or `Graphviz`_.
The visualisation style is controlled by passing instances of the classes
:cpp:class:`mod::graph::Printer` (C++) and :py:class:`mod.GraphPrinter` (Python)
to the printing functions.
The drawing style is inspired by `ChemFig`_ and `Open Babel`_.
See also :doc:`/postmod/postmod`.


.. _graph-dot:

DOT (Graph)
###########

The DOT format (from `Graphviz`_) is used for generating vertex coordinates for the Tikz format,
when `Open Babel`_ can not be used.


.. _graph-gml:

GML (Graph)
############

A graph can be specified as :ref:`GML` by giving a list of vertices and edges with the key ``graph``.
The following grammar exemplifies the required key-value structure.

.. productionlist:: GraphGMLGrammar
   graphGML: 'graph [' (`node` | `edge`)* ']'
   node: 'node [ id' `int` 'label' `quoteEscapedString` ']'
   edge: 'edge [ source' `int` 'target' `int` 'label' `quoteEscapedString` ']'

Note though that list elements can appear in any order.


.. _graph-smiles:

SMILES
######

The `Simplified molecular-input line-entry system` is a line notation for molecules.
MØD can load most SMILES strings, and converts them internally to labelled graphs.
For graphs that are sufficiently molecule-like, a SMILES string can be generated.
The generated strings are canonical in the sense that the same version of MØD will print
the same SMILES string for isomorphic molecules.

The reading of SMILES strings is based on the `OpenSMILES <http://www.opensmiles.org/>`_
specification, but with the following notes/changes.

- Only single SMILES strings are accepted, i.e., not multiple strings separated by white-space.
- The specical dot "bond" (``.``) is not allowed.
- Isotope information is ignored.
- Chirality information is ignored.
- Up and down bonds are regarded as implicit bonds, i.e., they might represent either a sngle bond
  or an aromatic bond. The stereo information is ignored.
- Atom classes are ignored.
- Wildcard atoms (specified with ``*``) are converted to vertices with label ``*``.
  Any other information on the vertex is ignored (e.g., the charge in ``[*-]``).
- Only charges from :math:`-9` to :math:`9` are allowed.
- The bond type ``$`` is currently not allowed.
- Aromaticity can only be specified using the bond type ``:``
  or using the special lower case atoms.
  I.e., ``c1ccccc1`` and ``C1:C:C:C:C:C:1`` represent the same molecule,
  but ``C1=CC=CC=C1`` is a different molecule.
- The final graph will conform to the molecule encoding scheme described below.
- Implicit hydrogens are added following a more complicated procedure.
- A bracketed atom can have a radical by writing a dot (``.``) between the position of the
  charge and the position of the class.

The written SMILES strings are intended to be canonical and may not conform to any "prettyness" standards.

Implicit Hydrogen Atoms
-----------------------

When SMILES strings are written they will use implicit hydrogens whenever they can be inferred when reading the string.
For the purposes of implicit hydrogens we use the following definition of valence for an atom.
The valence of an atom is the weighted sum of its incident edges, where single (``-``) and aromatic (``:``) bonds have weight 1, double bounds (``=``) have weight 2,
and triple bonds (``#``) have weight 3.
If an atom has an incident aromatic bond, its valence is increased by 1.
The atoms that can have implicit hydrogens are B, C, N, O, P, S, F, Cl, Br, and I.
Each have a set of so-called `normal` valences as shown in the following table.
The atoms N and S additionally have certain sets of incident edges that are also considered `normal`, which are also listed in the table.

=============  =====================================================================
Atom           Normal Valences and Neighbourhoods
=============  =====================================================================
B              3
C              4
N              3, 5, :math:`\{-, :, :\}`, :math:`\{-, -, =\}`, :math:`\{:, :, :\}`
O              2
P              3, 5
S              2, 4, 6, :math:`\{:, :\}`
F, Cl, Br, I   1
=============  =====================================================================

If the set of incident edges is listed in the table, then no hydrogens are added.
If the valence is higher than the highest normal valence, then no hydrogens are added.
Otherwise, hydrogens are added until the valence is at the next higher normal valence.

When writing SMILES strings the inverse procedure is used.


.. _graph-graphDFS:

GraphDFS
########

The GraphDFS format is intended to provide a convenient line notation for general undirected
labelled graphs. Thus it is in many aspects similar to SMILES strings, but a string being both
a valid SMILES string and GraphDFS string **may not represent the same graph**.
The semantics of ring-closures/back-edges are in particular not the same.

Grammar
-------

.. productionlist:: GraphDFSGrammar
   graphDFS: `chain`
   chain: `vertex` `evPair`*
   vertex: (`labelVertex` | `ringClosure`) `branch`*
   evPair: `edge` `vertex`
   labelVertex: '[' `bracketEscapedString` ']' [ `defRingId` ]
              : `implicitHydrogenVertexLabels` [ `defRingId` ]
   implicitHydrogenVertexLabels: 'B' | 'C' | 'N' | 'O' | 'P' | 'S' | 'F' | 'Cl' | 'Br' | 'I'
   defRingId: `unsignedInt`
   ringClosure: `unsignedInt`
   edge: '{' `braceEscapedString` '}'
       : `shorthandEdgeLabels`
   shorthandEdgeLabels: '-' | ':' | '=' | '#' | ''
   branch: '(' `evPair`+ ')'

A :token:`bracketEscapedString` and :token:`braceEscapedString` are zero or more characters
except respectively ``]`` and ``}``. To have these characters in each of their strings
they must be escaped, i.e., ``\]`` and ``\}`` respectively.

The parser additionally enforces that a :token:`defRingId` may not be a number which has
previously been used.
Similarly, a :token:`ringClosure` may only be a number which has previously occured in a
:token:`defRingId`.

A vertex specified via the :token:`implicitHydrogenVertexLabels` rule will potentially have
ekstra neighbours added after parsning. The rules are the exact same as for implicit hydrogen
atoms in :ref:`graph-smiles`.


Semantics
---------

A GraphDFS string is, like the SMILES strings, an encoding of a depth-first traversal of the
graph it encodes.
Vertex labels are enclosed in square brackets and edge labels are enclosed in curly brackets.
However, a special set of labels can be specified without the enclosing brackets.
An edge label may additionally be completely omitted as a shorthand for a dash (``-``).

A vertex can have a numeric identifier, defined by the :token:`defRingId` non-terminal.
At a later stage this identifier can be used as a vertex specification to specify a back-edge
in the depth-first traversal.
Example: ``[v1]1-[v2]-[v3]-[v4]-1``, specifies a labelled :math:`C_3`
(which equivalently can be specified shorter as ``[v1]1[v2][v3][v4]1``).

A :token:`vertex` being a :token:`ringClosure` can never be the first vertex in a string, and
is thus preceded with a :token:`edge`. As in a depth-first traversal, such a back-edge is a
kind of degenerated branch. Example: ``[v1]1[v2][v3][v4]1[v5][v6]1``, this specifies a graph
which is two fused :math:`C_4` with a common edge (and not just a common vertex).

.. warning:: The semantics of back-edges/ring closures are **not** the same as in SMILES strings.
   In SMILES, a pair of matching numeric identifiers denote the individual back-edges.

A branch in the depth-first traversal is enclosed in parentheses.

Abstracted Molecules
--------------------

The short-hand labels for vertices and edges makes it easier to specify partial molecules
than using :ref:`GML <graph-gml>` files.

As example, consider modelling Acetyl-CoA in which we wish to abstract most of the CoA part.
The GraphDFS string ``CC(=O)S[CoA]`` can be used and we let the library add missing hydrogen
atoms to the vertices which encode atoms. A plain CoA molecule would in this modelling be
``[CoA]S``, or a bit more verbosely as ``[CoA]S[H]``.

The format can also be used to create completely abstract structures
(it can encode any undirected labelled graph), e.g., RNA strings.
Note that in this case it may not be appropriate to add "missing" hydrogen atoms.
This can be controlled by an optional parameter to the loading function.
