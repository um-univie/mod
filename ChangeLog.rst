.. cpp:namespace:: mod
.. py:currentmodule:: mod

Changes
#######

v0.9.0 (2019-08-02)
===================

Incompatible Changes
--------------------

- Change to CMake as build system.
  See :ref:`installation` for how to build the package,
  or used it as a submodule in another CMake project.
- Now requires v0.4 of
  `GraphCanon <https://github.com/jakobandersen/graph_canon>`__
  (and `PermGroup <https://github.com/jakobandersen/perm_group>`__).
- :cpp:func:`dg::DG::getGraphDatabase` now returns a :cpp:any:`std::vector`
  instead of a :cpp:any:`std::unordered_set`.
- Hide internal symbols in the library to increase optimization opportunities,
  and hide symbols in library dependencies.
  Libraries linking against libmod may stop linking, but configuration options
  has been added to disable symbol hiding.


New Features
------------

- :py:func:`dgDump`/:cpp:func:`dg::DG::dump` should now be much, much faster
  in parsing the input file and loading the contained derivation graph.
- :py:func:`dgRuleComp`/:cpp:func:`dg::DG::ruleComp` should now be much faster
  during calculation.
- Added :py:func:`Graph.instantiateStereo`/:cpp:func:`graph::Graph::instantiateStereo`.
- Added :py:func:`rngReseed`/:cpp:func:`rngReseed`.


Bugs Fixed
----------

- Fixed off-by-one error in DG dump loading, :py:func:`dgDump`/:cpp:func:`dg::DG::dump`.
- Fixed issues with ``auto`` in function signatures which is not yet in the C++ standard.


Other
-----

- The functions :py:func:`prefixFilename`, :py:func:`pushFilePrefix`, and :py:func:`popFilePrefix`
  used by the :py:func:`include` function are now documented.
  A new class :py:class:`CWDPath` has been added.
- Use interprocedural/link-time optimization as default.
  It can be disabled with a configuration option.


v0.8.0 (2019-04-04)
===================

Incompatible Changes
--------------------

- Now requires v0.3 of
  `GraphCanon <https://github.com/jakobandersen/graph_canon>`__
  (and `PermGroup <https://github.com/jakobandersen/perm_group>`__).
- ``graph::Graph::getMolarMass``/``Graph.molarMass`` has been removed.
- Python interface: remove auto-generated hash-functions from all classes.
  Note, most code broken by this was already silemtly broken.
- Python interface: consistently disable all custom attributes on all classes.
- Removed ``dg::Strategy::GraphState::getHyperEdges``/``DGStratGraphState.hyperEdges``.
  Use the graph interface of :cpp:any:`dg::DG`/:py:obj:`DG` instead.
- All atoms, including hydrogens, are now present with ids in strings from
  :cpp:any:`graph::Graph::getSmilesWithIds`/:py:obj:`Graph.smilesWithIds`.
- :cpp:any:`dg::DG::print`/:py:obj:`DG.print` now returns a pair of strings,
  instead of just one string. The first entry is the old return value.
  The second entry is the tex-file to depend on for layout coordinates.
- SMILES parsing: disallow isotope 0 as it is equivalent to not specifying an isotope.
- All classes in the Python interface without a custom hash function has their
  hash function removed. This is to prevent inconsistencies between hash and equality.


New Features
------------

- Added support for isotopes (see :ref:`mol-enc`).
- Added :cpp:any:`graph::Graph::getExactMass`/:py:obj:`Graph.exactMass`.
- Added optional ``printInfo`` parameter to
  :cpp:any:`dg::DG::calc`/:py:obj:`DG.calc`
  to allow disabling of messages to stdout during calculation.
- The graph interface on :cpp:any:`dg::DG`/:py:obj:`DG` can now be used before and during
  calculation.
- Added include of the PGFPlots package in the summary preamble.
- Added :cpp:any:`AtomId::symbol`/:py:obj:`AtomId.symbol`.
- Added an ``add`` parameter to :py:obj:`graphGMLString`, :py:obj:`graphGML`,
  :py:obj:`graphDFS`, :py:obj:`smiles`, :py:obj:`ruleGMLString`, and :py:obj:`ruleGML`.
  It controls whether the graph/rule is appended to :py:obj:`inputGraphs`/:py:obj:`inputRules`
  or not. It defaults to ``True``.
- Add :cpp:any:`graph::Graph::getGraphDFSWithIds`/:py:obj:`Graph.graphDFSWithIds`
  for getting a string annotated with the internal vertex ids in form of the class labels.
  This mirrors the previously added :cpp:any:`graph::Graph::getSmilesWithIds`/:py:obj:`Graph.smilesWithIds`.
- Improve error messages from GML parsing of lists.
- Changed the return type of :cpp:func:`dg::DG::getGraphDatabase` from a `std::set` to a `std::unordered_set`.
- :cpp:func:`dg::DG::HyperEdge::print`/:py:func:`DGHyperEdge.print` now returns a list of file data.
- The vertices and edges of all graph interfaces now have a conversion to bool:

  - :cpp:class:`graph::Graph::Vertex`/:py:class:`GraphVertex`,
    :cpp:class:`graph::Graph::Edge`/:py:class:`GraphEdge`
  - :cpp:class:`rule::Rule::Vertex`/:py:class:`RuleVertex`,
    :cpp:class:`rule::Rule::Edge`/:py:class:`RuleEdge`
  - :cpp:class:`rule::Rule::LeftGraph::Vertex`/:py:class:`RuleLeftGraphVertex`,
    :cpp:class:`rule::Rule::LeftGraph::Edge`/:py:class:`RuleLeftGraphEdge`
  - :cpp:class:`rule::Rule::ContextGraph::Vertex`/:py:class:`RuleContextGraphVertex`,
    :cpp:class:`rule::Rule::ContextGraph::Edge`/:py:class:`RuleContextGraphEdge`
  - :cpp:class:`rule::Rule::RightGraph::Vertex`/:py:class:`RuleRightGraphVertex`,
    :cpp:class:`rule::Rule::RightGraph::Edge`/:py:class:`RuleRightGraphEdge`
  - :cpp:class:`dg::DG::Vertex`/:py:class:`DGVertex`,
    :cpp:class:`dg::DG::HyperEdge`/:py:class:`DGHyperEdge`

- The vertices of all graph interfaces now have a proper hash support.
- Added :cpp:func:`dg::Printer::setRotationOverwrite`/:py:func:`DGPrinter.setRotationOverwrite`
  and :cpp:func:`dg::Printer::setMirrorOverwrite`/:py:func:`DGPrinter.setMirrorOverwrite`.


Bugs Fixed
----------

- Throw :cpp:any:`InputError`/:py:obj:`InputError` when loading a DG dump
  when a rule in the dump can not be linked to a rule from the user.
- Fix molecule decoding of atoms with negative charge and a radical.
- Fix dangling reference bug in first-order term handling.
- Fix inifiinite loop bug in first-order term handling.
- Remove extraneous template parameter lists to make it compile on GCC 8.
- Fix the documentation of
  :py:obj:`Graph.minExternalId`, :py:obj:`Graph.maxExternalId`,
  :py:obj:`Rule.minExternalId`, and :py:obj:`Rule.maxExternalId`.
  It was not being rendered.
- Fixed documentation of the constructor for :cpp:class:`AtomData`.
- Fix dangling references in morphism callbacks.
- Make sure Open Babel is not called in some cases where it is not needed.
- Find the library file for Boost.Python for Boost >= 1.67.
- Fix ambiguity between variadic arguments and function parameter packs,
  making term morphisms and stereo morphisms slow.
- Removed sanity check from GraphDFS loading which dominated the run time.
- Document :py:obj:`inputGraphs` and :py:obj:`inputRules`.


Other
-----

- Now compiles with ``-fno-stack-protector`` (some OS distributions messes with default flags).
- The Makefile from ``mod --get-latex`` now cleans ``.vrb``, ``.snm``, and ``.nav`` files as well.


v0.7.0 (2018-03-08)
===================

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


v0.6.0 (2016-12-22)
===================

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


v0.5.0 (2016-03-07)
===================

Initial public version.
