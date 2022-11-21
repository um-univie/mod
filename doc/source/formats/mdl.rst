.. _graph-mdl:

MOL and SD
##########

MØD can load graphs stored in the
`CT File <https://en.wikipedia.org/wiki/Chemical_table_file>`__ formats
MOL (single structure) and SD (multiple structures).
See the loading functions in :cpp:class:`graph::Graph`/:py:class:`Graph` for
the API.
The loaded structures are converted to labelled graphs according to a specific
:ref:`molecule encoding <mol-enc>`.

The reading of structures is based on the
`published specification <https://web.archive.org/web/20210219065450/https://discover.3ds.com/sites/default/files/2020-08/biovia_ctfileformats_2020.pdf>`__,
but with the following notes/changes.

- the :cpp:class:`MDLOptions`/:py:class:`MDLOptions` can be used to customize
  the loading procedure. 
- radical value 2 is converted to ``.`` in the vertex labels.
- the atom symbols "LP" and "L" are used as is, as an atom with an abstract
  label.
- the atom symbols "A", "Q", and "*" are all considered as wildcard atoms and
  are converted to vertices with label ``*``.
  See the use of :ref:`first-order terms <graph-model-terms>` as labels.
- the bond orders 5, 6, and 7 for constrained wildcard bonds are converted
  to edges with labels starting with ``_Q``, i.e., term variables.
  See the use of :ref:`first-order terms <graph-model-terms>` as labels.
- the bond order 8 for unconstrianed bonds are converted to edges with label
  ``*``.
  See the use of :ref:`first-order terms <graph-model-terms>` as labels.
