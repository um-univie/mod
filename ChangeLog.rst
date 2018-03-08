.. cpp:namespace:: mod
.. py:module:: mod

Changes
#######

Release 0.7.0 (2018-03-08)
==========================

Incompatible Changes
--------------------

- Boost >= 1.64 is now required.
- Two new libraries,
  `GraphCanon <https://github.com/jakobandersen/graph_canon>`__ and
  `PermGroup <https://github.com/jakobandersen/perm_group>`__, are now required dependencies.
- Sphinx 1.7.1 is now required for building the documentation.
- :cpp:any:`dg::DG::HyperEdge::print`/:py:obj:`DGHyperEdge.print`
  now also takes an argument for colouring vertices/edges
  that are not matched by the rule. The default is now that matched vertices/edges
  are the default colour, while those that are not matched are grey.
- Most of the outer interface headers have now been moved to subfolders,
  and their content has been moved to corresponding namespaces.
  Several classes has been slightly renamed during the move,
  and some headers has been split into multiple header files.
- The previously deprecated class ``DerivationRef`` has now been removed along with
  ``DG.derivations`` in the Python interface.
- The previously deprecated method ``DG.vertexGraphs`` in the Python interface
  has been removed.


New Features
------------

- Added functions to map external ids of graphs and rules to internal vertices.
- Added functions to get vertex coordinates for rules.
- :cpp:any:`dg::DG::print`/:py:obj:`DG.print`
  now returns the name of the PDF-file that will be created in post-processing.
- Add :cpp:any:`dg::Printer::setWithInlineGraphs`/:py:obj:`DGPrinter.withInlineGraphs`
  to input raw tex files for graphs in nodes instead of compiled PDFs.
  The generated tex code for the graphs is different to ensure unique Tikz node names.
- Add ``inline`` as a special argument for the graph Latex macros to input the raw tex file,
  instead of including a compiled PDF.
- Add :cpp:any:`graph::Graph::getSmilesWithIds`/:py:obj:`Graph.smilesWithIds`
  for getting a string annotated with the internal vertex ids in form of the class labels.
- The automorphism group of each graph is now available.


Experimental New Features
-------------------------

- Vertex/edge labels in graphs/rules can now be interpreted as first-order terms.
  Syntactic unification is then performed during morphism finding.
  See where :cpp:any:`LabelSettings`/:py:obj:`LabelSettings` is being used.
  Each rule has an optional :cpp:any:`LabelType`/:py:obj:`LabelType`
  to signify whether it was designed for use with
  :cpp:any:`LabelType::String`/:py:obj:`LabelType.String` or
  :cpp:any:`LabelType::Term`/:py:obj:`LabelType.Term`.
  Some algorithms will check this property for safety reasons, but the check can be disbled.
- There is now a prototype-implementation of http://doi.org/10.1007/978-3-319-61470-0_4,
  for adding stereo-information to graphs/rules.
  Use :cpp:any:`LabelSettings`/:py:obj:`LabelSettings` objects to enable it.
  See the paper for examples on how to use it. The full framework will be implemented and
  documented in a future version.


Bugs Fixed
----------

- Multiple rules for the same derivation is now properly recorded.
- Fix documentation of :cpp:any:`rule::Rule::makeInverse`/:py:obj:`Rule.makeInverse`,
  it throws :cpp:any:`LogicError`/:py:obj:`LogicError`
  not :cpp:any:`InputError`/:py:obj:`InputError`.
- Set the name of a rule from :cpp:any:`rule::Rule::makeInverse`/:py:obj:`Rule.makeInverse`
  to something more descriptive.
- Fix graph/rule depiction bug with non-zero rotation.
- Fix DG dump loading to also load derivations with no rules.
- Don't crash when trying :cpp:any:`dg::DG::findVertex`/:py:obj:`DG.findVertex`
  with a graph not in the derivation graph.
- Don't crash when trying to print derivations with multiple rules.
- Fix documentation formatting errors.
- #2, throw exceptions from :cpp:any:`dg::DG::ruleComp`/:py:obj:`dgRuleComp`
  and :cpp:any:`dg::DG::calc`/:py:obj:`DG.calc` when isomorphic graphs are given.
- Throw more appropriate exception if :cpp:any:`dg::DG::print`/:py:obj:`DG.print`
  is called before :cpp:any:`dg::DG::calc`/:py:obj:`DG.calc`.
- Various issues in graph/rule depiction related to positioning of hydrogens, charges, etc.
- Build system: give better error messages if a file is given where a path is expected.
- The produced SMILES strings are now truely canonical, as the new
  `GraphCanon <https://github.com/jakobandersen/graph_canon>`__ library is used.
- Fix :cpp:any:`AtomData`/:py:obj:`AtomData` to properly print radicals on uncharged atoms.
- Throw more friendly exceptions when loading graphs/rules that have loop/parallel edges.


Other
-----

- The documentation now has a "Known Issues" section, describing an issue where
  post-processing may fail if ``pdflatex`` has been upgraded after installation


Release 0.6.0 (2016-12-22)
==========================

Incompatible Changes
--------------------

- A C++14 compiler is now required (e.g., GCC 5.1 or later).
- The required Boost version is now either 1.59 or at least 1.61.
  Version 1.60 do not work due to https://github.com/boostorg/python/issues/56.
- Make filenames in post-processing more Latex friendly.
- Rules specified in GML using ``constrainAdj`` must now enclose the operator in double quotes.  
- DG: remove most of the interface related to ``DerivationRef``.
  Use the graph interface instead. E.g.,

  - Deprecate ``DerivationRef``. They now interconvert with
    :cpp:any:`dg::DG::HyperEdge`/:py:obj:`DGHyperEdge`.
  - Change ``DG::getDerivationRef`` into :cpp:any:`dg::DG::findEdge`/:py:obj:`DG.findEdge`.
  - Make ``DG.derivations`` return the edges instead in the Python interface.
    It is removed in the C++ interface. It will be removed from Python in the future.
  - Remove ``DG::inDerivations`` and ``DG::outDerivations``.
  - Deprecate ``DG::vertexGraphs`` in Python, remove in C++.

- Move the graph interface for DG and Graph into separate headers:
  ``DGGraphInterface.h`` and ``GraphGraphInterface.h``.
- Move ``GraphPrinter`` into a separate header.
- Move ``DGPrinter`` and ``DGPrintData`` into a separate header.
- All SBML features have been removed from the library.
- The deprecated ``DG::printMatrix`` function has been removed.
- :cpp:any:`dg::DG::calc`/:py:obj:`DG.calc` will no longer print a message when it is done.
- :cpp:any:`dg::DG::print`/:py:obj:`DG.print` by default now only prints the hypergraph rendering.
  (For now, set ``Config::DG::printNonHyper`` to enable printing of the non-hypergraph rendering)
- :cpp:any:`graph::Graph::print`/:py:obj:`Graph.print` and
  :cpp:any:`rule::Rule::print`/:py:obj:`Rule.print` will now emit only one depiction when
  the two printers are equal.
- :cpp:any:`rule::Rule::print`/:py:obj:`Rule.print`, change the default colours used to indicate
  changes. Now different colours are used in L, K, R.
- :py:obj:`DGHyperEdge.print`, change the default match colour.
- Add < operator to vertices and edges of Graph, Rule, and DG.


New Features
------------

- :cpp:any:`rule::Rule::makeInverse`/:py:obj:`Rule.makeInverse`.
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
- Add :cpp:any:`graph::Printer::setRotation`/:py:obj:`GraphPrinter.rotation`.
  Internally computed coordinates will be rotated by the set amount of degrees.
 

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
