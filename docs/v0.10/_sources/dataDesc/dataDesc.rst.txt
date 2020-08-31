************
Data Formats
************

MØD utilises several data formats and encoding schemes.


.. _gml:

GML
###

MØD uses the Graph Modelling Language (GML) for general specification of graphs
and rules.
The parser recognises most of the published `specification
<http://www.fim.uni-passau.de/fileadmin/files/lehrstuhl/brandenburg/projekte/gml/gml-technical-report.pdf>`__,
with regard to syntax. The specific grammar is as follows.

.. productionlist:: gml
   GML: (`key` `value`)*
   key: `identifier`
   value: int
        : float
        : quoteEscapedString
        : `list`
   list: '[' (`key` `value`)* ']'
   identifier: a word matching the regex "[a-zA-Z][a-zA-Z0-9]*"

A ``quoteEscapedString`` is zero or more characters surrounded by double
quotation marks.  To include a ``\"`` character it must be escaped.  Tabs,
newlines, and backslashses can be written as ``\t``, ``\n``, and ``\\``.
GML code may have line comments, starting with ``#``.
They are ignored during parsing.


.. _graph-gml:

Graph
-----

A graph can be specified as :ref:`GML` by giving a list of vertices and edges
with the key ``graph``.
The following grammar exemplifies the required key-value structure.

.. productionlist:: gml
   graphGML: 'graph [' (`node` | `edge`)* ']'
   node: 'node [ id' int 'label' quoteEscapedString ']'
   edge: 'edge [ source' int 'target' int 'label' quoteEscapedString ']'

Note though that list elements can appear in any order.


.. _rule-gml:

Rule
----

A rule :math:`(L\leftarrow K\rightarrow R)` in :ref:`GML` format is specified
as three graph fragments; ``left``, ``context``, and ``right``.
From those
:math:`L` is constructed as ``left`` :math:`\cup` ``context``,
:math:`R` as ``right`` :math:`\cup` ``context``, and
:math:`K` as ``context`` :math:`\cup` (``left`` :math:`\cap` ``right``).
Each graph fragment is specified as a list of vertices and edges, similar to a
graph in GML format.
The key-value structure is exemplified by the following grammar.

.. productionlist:: gml
   ruleGML: 'rule ['
          :    [ 'ruleID' quoteEscapedString ]
          :    [ 'labelType' `labelType` ]
          :    [ `leftSide` ]
          :    [ `context` ]
          :    [ `rightSide` ]
          :    `matchConstraint`*
          : ']'
   labelType: 'string' | 'term'
   leftSide: 'left [' (`node` | `edge`)* ']'
   context: 'context [' (`node` | `edge`)* ']'
   rightSide: 'right [' (`node` | `edge`)* ']'
   matchConstraint: `adjacency`
   adjacency: 'constrainAdj ['
            :    'id' int
            :    'op "' `op` '"'
            :    'count' unsignedInt
            :    [ 'nodeLabels [' `labelList` ']' ]
            :    [ 'edgeLabels [' `labelList` ']' ]
            : ']'
   labelList: ('label' quoteEscapedString)*
   op: '<' | '<=' | '=' | '>=' | '>'

Note though that list elements can appear in any order.


.. include:: rule.rst

.. include:: graph.rst

.. include:: molEnc.rst

.. include:: term.rst

.. include:: dg.rst
