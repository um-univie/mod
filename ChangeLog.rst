Release 0.6.0 (2016-12-22)
==========================

Incompatible Changes
--------------------

- A C++14 compiler is now required (e.g., GCC 5.1 or later).
- The required Boost version is now either 1.59 or at least 1.61.
  Version 1.60 do not work due to https://github.com/boostorg/python/issues/56.
- Make filenames in post-processing more Latex friendly.
- Rules specified in GML using ``constrainAdj`` must now enclose the operator in double quotes.  
- DG: remove most of the interface related to ``DerivationRef``. Use the graph interface instead. E.g.,

  - Deprecate ``DerivationRef``. They now interconvert with ``DG::HyperEdge``.
  - Change ``DG::getDerivationRef`` into ``DG::findEdge``.
  - Make ``DG::derivations`` return the edges instead in the Python interface.
    It is removed in the C++ interface. It will be removed from Python in the future.
  - Remove ``DG::inDerivations`` and ``DG::outDerivations``.
  - Deprecate ``DG::vertexGraphs`` in Python, remove in C++.

- Move the graph interface for DG and Graph into separate headers:
  ``DGGraphInterface.h`` and ``GraphGraphInterface.h``.
- Move GraphPrinter into a separate header.
- Move DGPrinter and DGPrintData into a separate header.
- All SBML features have been removed from the library.
- The deprecated DG::printMatrix function has been removed.
- DG::calc will no longer print a message when it is done.
- DG::print by default now only prints the hypergraph rendering.
  (For now, set Config::DG::printNonHyper to enable printing of the non-hypergraph rendering)
- Graph::print and Rule::print will now emit only one depiction when the two printers are equal.
- Rule::print, change the default colours used to indicate changes. Now different colours are used in L, K, R.
- DGHyperEdge::print, change the default match colour.
- Add < operator to vertices and edges of Graph, Rule, and DG.


New Features
------------

- ``Rule::makeInverse``.
- Reimplementation of GML parsing. It is now less strict with respect to ordering.
- Rule application constraint that checks the shortest path between two given vertices.
- Interactive mode for the wrapper script (option ``-i``).
  It will use IPython as interpreter if it is available.
- The molecule model now includes radicals. The SMILES format has been extended to support
  radicals as well.
- Plugin infrastructure to load additional Python modules when using the wrapper script.
- Graph interface for rules: for a rule :math:`L \leftarrow K\rightarrow R``, the three graphs
  can be accessed. The rule it self acts as the graph that is the pushout of the rule span.
- Graph loading: the ids used in GML and GraphDFS, as well as the class labels in SMILES can now
  be converted into vertices for the loaded graphs. If the class labels of a SMILES string are not
  unique, then none of them are available for querying.
- Add ``-v`` as alias for ``--version`` in the wrapper script.
- Add quite mode, ``-q``, to the wrapper script.
- Add ``rotation`` to GraphPrinter. Internally computed coordinates will be rotated by ``rotation`` degrees.
 

Bugs Fixed
----------

- ``operator<<`` for Derivation: only try to print the rule if there is one.
- Properly throw an exception when graph GML parsing fails.
- Don't throw an exception while throwing an exception when graphs are disconnected.
- Fix bug in checking of certain ``constrainAdj`` during certain types of rule composition.
- Properly handle empty vertex/hyperedge ranges for DGs. Thanks to Robert Haas for reporting.


Release 0.5.0 (2016-03-07)
==========================

Initial public version.
