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
