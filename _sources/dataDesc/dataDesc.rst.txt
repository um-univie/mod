************
Data Formats
************

MØD utilises several data formats and encoding schemes.


.. _gml:

GML
###

MØD uses the Graph Modelling Language (GML) for general specification of graphs and rules.
The parser recognises most of the published `specification <http://www.fim.uni-passau.de/fileadmin/files/lehrstuhl/brandenburg/projekte/gml/gml-technical-report.pdf>`__, with regard to syntax.
The specific grammar is as follows.

.. productionlist:: GML
   GML: (`key` `value`)*
   key: `identifier`
   value: `int`
        : `double`
        : `quoteEscapedString`
        : `list`
   list: '[' (`key` `value`)* ']'

A :token:`quoteEscapedString` is zero or more characters surrounded by double quotation marks.
To include a ``\"`` character it must be escaped.
Tabs, newlines, and backslashses can be written as ``\t``, ``\n``, and ``\\``.
An identifier must match the regular expression ``[a-zA-Z][a-zA-Z0-9]*``
GML code may have line comments, starting with ``#``. They are ignored during parsing.


.. include:: rule.rst

.. include:: graph.rst

.. include:: molEnc.rst
