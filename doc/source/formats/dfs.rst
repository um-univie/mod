.. _format-dfs:

DFS Line Notation
#################

The DFS formats are intended to provide a convenient line
notation for general undirected labelled graphs and rules.
Thus it is in many aspects similar to SMILES strings and reaction SMILES
strings, but a string being both a valid (reaction SMILES) string and
GraphDFS/RuleDFS string **does not mean they represent the same objects**.
In particular, the semantics of ring-closures/back-edges are not the same.

.. _format-graphDFS:

GraphDFS
========

.. productionlist:: graphDFS
   graphDFS: `chain`
   chain: `vertex` `evPair`*
   vertex: (`labelVertex` | `ringClosure`) `branch`*
   evPair: `edge` `vertex`
   labelVertex: '[' bracketEscapedString ']' [ `defRingId` ]
              : `implicitHydrogenVertexLabels` [ `defRingId` ]
   implicitHydrogenVertexLabels: 'B' | 'C' | 'N' | 'O' | 'P' | 'S' | 'F' \
   | 'Cl' | 'Br' | 'I'
   defRingId: unsignedInt
   ringClosure: unsignedInt
   edge: '{' braceEscapedString '}'
       : `shorthandEdgeLabel`
   shorthandEdgeLabel: '-' | ':' | '=' | '#' | '.' | ''
   branch: '(' `evPair`+ ')'

A ``bracketEscapedString`` and ``braceEscapedString`` are zero or more
characters except respectively ``]`` and ``}``. To have these characters in
each of their strings they must be escaped, i.e., ``\]`` and ``\}``
respectively.

Whitespace is ignored, except inside ``bracketEscapedString`` and
``braceEscapedString``.

The parser additionally enforces that a :token:`~graphDFS:defRingId` may not be
a number which has previously been used.
Similarly, a :token:`~graphDFS:ringClosure` may only be a number which has
previously occured in a :token:`~graphDFS:defRingId`.

A vertex specified via the :token:`~graphDFS:implicitHydrogenVertexLabels` rule
will potentially have ekstra neighbours added after parsning. The rules are the
exact same as for implicit hydrogen atoms in :ref:`graph-smiles`.

Semantics
---------

A GraphDFS string is, like the SMILES strings, an encoding of a depth-first
traversal of the graph it encodes. Vertex labels are enclosed in square
brackets and edge labels are enclosed in curly brackets. However, a special
set of labels can be specified without the enclosing brackets.
An edge label may additionally be completely omitted as a shorthand for a dash
(``-``).

A vertex can have a numeric identifier, defined by the
:token:`~graphDFS:defRingId` non-terminal.
At a later stage this identifier can be used as a vertex specification to
specify a back-edge in the depth-first traversal.
Example: ``[v1]1-[v2]-[v3]-[v4]-1``, specifies a labelled :math:`C_4`
(which equivalently can be specified shorter as ``[v1]1[v2][v3][v4]1``).

A :token:`~graphDFS:vertex` being a :token:`~graphDFS:ringClosure` can never be
the first vertex in a string, and is thus preceded with a
:token:`~graphDFS:edge`. As in a depth-first traversal, such a back-edge is a
kind of degenerated branch. Example: ``[v1]1[v2][v3][v4]1[v5][v6]1``, this
specifies a graph which is two fused :math:`C_4`,
:math:`v_1, v_2, v_3, v_4` and :math:`v_4, v_5, v_6, v_1`,
with a common edge, :math:`(v_1, v_4)`.

.. warning:: The semantics of back-edges/ring closures are **not** the same as
	in SMILES strings. In SMILES, a pair of matching numeric identifiers denote
	the individual back-edges.

A branch in the depth-first traversal is enclosed in parentheses.

The :token:`~graphDFS:shorthandEdgeLabel` ``.`` indicates a non-edge,
i.e., a jump to a new vertex without creating an edge.
For example ``[v1].[v2]`` encodes a graph with two vertices and no edges,
while ``[v1]{.}[v2]`` encodes a graph with two vertcies connected with an edge
with label ``.``.

Abstracted Molecules
--------------------

The short-hand labels for vertices and edges makes it easier to specify partial
molecules than using :ref:`GML <graph-gml>` files.

As example, consider modelling Acetyl-CoA in which we wish to abstract most of
the CoA part. The GraphDFS string ``CC(=O)S[CoA]`` can be used and we let the
library add missing hydrogen atoms to the vertices which encode atoms. A plain
CoA molecule would in this modelling be ``[CoA]S``, or a bit more verbosely as
``[CoA]S[H]``.

The format can also be used to create completely abstract structures
(it can encode any undirected labelled graph), e.g., RNA strings.
Note that in this case it may not be appropriate to add "missing" hydrogen
atoms. This can be controlled by an optional parameter to the loading function.


.. _format-ruleDFS:

RuleDFS
=======

The rule format builds on the graph format by using two GraphDFS strings to
encode a rule:

.. productionlist:: graphDFS
   ruleDFS: [ `graphDFS` ] '>>' [ `graphDFS` ]

The two (possibly empty) GraphDFS strings encode the left-hand and right-hand
side of a rule, with the vertex IDs being used to relate them.
That is, a pair of vertices in the left side and right side with the same ID
will be identified and the vertex put in the context graph of the rule as well.
A similar pair of edges where both end-points are in the context graph will be
put in the context graph as well.

Examples:

- ``>>``: the empty rule.
- ``[A]>>``: a rule with a single vertex in :math:`L`, and empty :math:`K` and
  :math:`R`.
- ``[A]>>[B]``: a rule with empty :math:`K` but with a vertex in :math:`L`
  which is removed by the rule, and a vertex in :math:`R` being created by the
  rule.
- ``[A]1>>[B]1``: a rule with a vertex changing label from "A" to "B".

.. note:: Currently it is not possible to use vertices with implicit hydrogens
	in RuleDFS.
