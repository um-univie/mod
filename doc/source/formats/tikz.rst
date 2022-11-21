Tikz
####

Both graphs and rules are visualized through :ref:`PostMØD <mod_post>` by the
library generating `Tikz <http://www.ctan.org/pkg/pgf>`__ code and compiling it
with Latex.

The visualisation style is controlled by passing instances of
:cpp:class:`mod::graph::Printer`/:py:class:`mod.GraphPrinter`
to the printing functions.
The drawing style is inspired by `ChemFig <http://www.ctan.org/pkg/chemfig>`__
and `Open Babel <http://openbabel.org>`__.

The coordinates for the layout is either generated using Open Babel when the
graphs a chemical enough, but otherwise `Graphviz <http://www.graphviz.org>`__
is invoked to generate coordinates.

For visualizing a rule or DPO diagram, the position of vertices is used to
indicate how morphisms map vertices to each other.
