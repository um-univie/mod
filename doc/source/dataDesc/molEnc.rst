
.. _mol-enc:

Molecule Encoding
#################

There is no strict requirement that graphs encode molecules, however several optimizations
are in place when they do.
The following describes how to encode molecules as undirected, simple, labelled graphs and thus
when the library assumes a graph is a molecule.

Edges / Bonds
-------------

An edge encodes a chemical bond if and only if its label is listed in the table below.

====== ==================
Label  Interpretation  
====== ==================
``-``  Single bond     
``:``  "Aromatic" bond 
``=``  Double bond     
``#``  Triple bond     
====== ==================

Vertices / Atoms
----------------

A vertex encodes an atom with a charge if and only if its label conforms to the following grammar.

.. productionlist:: VertexLabel
   vertexLabel: [ isotope ] `atomSymbol` [ charge ] [ radical ]
   isotope: `unsignedInt`
   charge: `singleDigit` ('-' | '+')
   radical: '.'

With :token:`atomSymbol` being a properly capitalised atom symbol.

Currently there are no valence requirements for a graph being recognised as a molecule.
