.. _rule-tikz:

Tikz (Rule)
###########

This format is used for visualising rules similarly to how the :ref:`graph-tikz` format is used
for graphs. A rule is depicted as its span :math:`(L\leftarrow K\rightarrow R)` with the vertex
positions in the plane indicating the embedding of :math:`K` in :math:`L` and :math:`R`.
Additionally, :math:`L\backslash K` and :math:`R\backslash K` are shown in different colour in
:math:`L` and :math:`R` respectively.


.. _rule-dot:

DOT (Rule)
##########

The DOT format (from `Graphviz`_) is used for generating vertex coordinates for the Tikz format,
when `Open Babel`_ can not be used.


.. _rule-gml:

GML (Rule)
##########

A rule :math:`(L\leftarrow K\rightarrow R)` in :ref:`GML` format is specified as three graphs;
:math:`L\backslash K`, :math:`K`, and :math:`R\backslash K`.
Each graph is specified a a list of vertices and edges, similar to a graph in GML format.
The key-value structure is exemplified by the following grammar.

.. productionlist:: RuleGMLGrammar
   ruleGML: 'rule [' 
          :    [ `ruleId` ]
          :    [ `leftSide` ]
          :    [ `context` ]
          :    [ `rightSide` ]
          :    `matchConstraint`*
          : ']'
   ruleId: 'ruleID' `quoteEscapedString`
   leftSide: 'left [' (`node` | `edge`)* ']'
   context: 'context [' (`node` | `edge`)* ']'
   rightSide: 'right [' (`node` | `edge`)* ']'
   node: 'node [ id' `int` 'label' `quoteEscapedString` ']'
   edge: 'edge [ source' `int` 'target' `int` 'label' `quoteEscapedString` ']'
   matchConstraint: `adjacency`
   adjacency: 'constrainAdj ['
            :    'id' `int`
            :    'op "' `op` '"'
            :    'count' `unsignedInt`
            :    [ 'nodeLabels [' `labelList` ']' ]
            :    [ 'edgeLabels [' `labelList` ']' ]
            : ']'
   labelList: ('label' `quoteEscapedString`)*
   op: '<' | '<=' | '=' | '>=' | '>'

Note though that list elements can appear in any order.
