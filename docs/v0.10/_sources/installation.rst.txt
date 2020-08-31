.. _installation:

Installation
============


From a Git Repository
---------------------

After a checkout of the desired version, do::

	git submodule update --init --recursive
	./bootstrap.sh

This is needed to first fetch certain dependencies and second
to generate build files, extract the API documentation,
and create the file ``VERSION`` based on the current commit.

See :ref:`source-build` on how to then build the package.


As Dependency of Another CMake Project
--------------------------------------

MØD supports use via ``add_subdirectory`` in CMake.
The target ``mod::libmod`` is exported,
which can be used with ``target_link_libraries`` to link against libMØD.
The version is in the variable ``mod_VERSION``.
Note that running ``./bootstrap.sh`` is still needed if the
source is a repository clone (e.g., a Git submodule).


.. _source-build:

From a Source Archive
---------------------

The package is build and installed from source as a CMake project.
Generally that means something like::

	mkdir build
	cd build
	cmake ../ <options>
	make -j <n>
	make install

A source archive can also be created with ``make dist``.

The following is a list of commonly used options for ``cmake``,
and additional options specific for MØD.
See also :ref:`dependencies` for elaboration on some of them.

- ``-DCMAKE_INSTALL_PREFIX=<prefix>``, set a non-standard installation directory.
  Note also that the
  `GNUInstallDirs <https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html>`__
  module is used.
- ``-DCMAKE_BUILD_TYPE=<build type>``, set a non-standard build type.
  The default is `RelWithDebInfo <https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=build_type#variable:CMAKE_BUILD_TYPE>`__.
  An additional build type ``OptDebug`` is available which adds the compilation flags ``-g -O3``.
- ``-DCMAKE_PREFIX_PATH=<paths>``, set a ``;``-separated list of paths used for finding most dependencies.
  The paths should generally be the prefixes specified when the dependency was installed.
  See also https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html#variable:CMAKE_PREFIX_PATH.
- ``-DBUILD_DOC=on``, whether to build documentation or not.
  This is forced to ``off`` when used via ``add_subdirectory``.
- ``-DBUILD_POST_MOD=on``, whether to build the post-processor or not.
- ``-DBUILD_PY_MOD=on``, whether to build the Python bindings or not.
- ``-DBUILD_EXAMPLES=off``, whether to build and allow running of examples as
  tests or not.
  This is forced to ``off`` when used via ``add_subdirectory``.
  This is forced to ``off`` when ``BUILD_TESTING`` is ``off``.
- ``-DBUILD_TESTING=off``, whether to allow test building or not.
  This is forced to ``off`` when used via ``add_subdirectory``.
  When ``on`` the tests can be build with ``make tests`` and run with ``ctest``.
- ``-DBUILD_TESTING_SANITIZERS=on``, whether to compile tests with sanitizers or not.
  This has no effect with code coverage is enabled.
- ``-DBUILD_COVERAGE=off``, whether to compile code and run tests with GCov.
  When ``on`` the sanitizers on tests will be disabled.
  After building the tests, execute ``make coverage_collect`` without parallel jobs to run tests.
  Afterwards, execute ``make coverage_build`` to compile the code coverage report.
- ``-DENABLE_SYMBOL_HIDING=on``, whether symbols internal to the library are hidden or not.
  Disabling this option may degrade performance, and should only be done while developing extensions to the C++ library.
- ``-DENABLE_DEP_SYMBOL_HIDING=on``, whether symbols from library dependencies are hidden or not.
  Disabling this option may make it slower to load the library at runtime.
- ``-DENABLE_IPO=on``, whether to use link-time optimization or not.
  Disabling this option may degrade performance, but speed up linking time.
  As default the link-time optimizer will only use 1 thread.
  To use more threads, e.g., 7, use the following options for configuration
  ``-DCMAKE_MODULE_LINKER_FLAGS="-flto=7" -DCMAKE_SHARED_LINKER_FLAGS="-flto=7"``, when using GCC as the compiler.
- ``-DUSE_NESTED_GRAPH_CANON=on``, whether to use the dependency GraphCanon from the Git submodule or not.
- ``-DWITH_OPENBABEL=on``, whether to enable/disable features depending on Open Babel.


.. _dependencies:

Dependencies
------------

- This documentation requires (``-DBUILD_DOC=on``):

  - A supported version of `Sphinx <http://sphinx-doc.org>`__.

- libMØD:

  - A C++ compiler with reasonable C++14 support is needed. GCC 6.1 or later should work.
  - `Boost <http://boost.org>`__ dev >= 1.64
    (use ``-DBOOST_ROOT=<path>`` for non-standard locations).
  - `GraphCanon <https://github.com/jakobandersen/graph_canon>`__ >= 0.4.
    This is fulfilled via a Git submodule (make sure to do ``git submodule update --init --recursive``),
    but if another source is needed, set ``-DUSE_NESTED_GRAPH_CANON=off``.
  - (optional) `Open Babel`_ dev, >= 2.3.2 (``-DWITH_OPENBABEL=on``).

- PyMØD (``-DBUILD_PY_MOD=on``):

  - Python 3 dev
  - Boost.Python built with Python 3
  - (Optional) IPython 3

- PostMØD (``-DBUILD_POST_MOD=on``):

  - ``pdflatex`` available in the ``PATH`` or in ``CMAKE_PROGRAM_PATH``,
    with not too old packages (Tex Live 2012 or newer should work).
  - ``pdf2svg`` available in the ``PATH`` or in ``CMAKE_PROGRAM_PATH``.
  - ``dot`` and ``neato`` from Graphviz available in the ``PATH`` or in ``CMAKE_PROGRAM_PATH``.
    They must additionally be able to load SVG files and output as both SVG and PDF files (all via cairo).
    That is, in the output of ``dot -P`` the following edges must exist:

    - ``cairo_device_svg -> output_svg``
    - ``cairo_device_pdf -> output_pdf``
    - ``rsvg_loadimage_svg -> render_cairo``


Ubuntu
^^^^^^

Install the following packages.

.. - (>= Ubuntu 16.10) Boost: ``libboost-regex-dev libboost-system-dev``.
.. - (>= Ubuntu 16.10) Boost.Python: ``python3-dev libboost-python-dev``.

- Boost, Boost.Python: ``python3-dev``, and then see :ref:`install_boost_python`.
- (>= Ubuntu 14.04) Open Babel: ``libopenbabel-dev``.
- (< Ubuntu 14.04) Open Babel: install from source.
- ``pdflatex``: ``texlive-full`` (less can do it, will be refined in the future).
- ``pdf2svg``: ``pdf2svg``.
- Graphviz: The Ubuntu package unfortunately does not include the plugin for converting SVG to PDF via rsvg and cairo.
  ``librsvg2-dev libpango1.0-dev``, and see :ref:`install_graphviz`.


Fedora
^^^^^^

Install the following packages.

- (>= Fedora 27) Boost: ``boost-devel boost-regex boost-system``.
- (>= Fedora 27) Boost.Python: ``python3-devel boost-python3-devel``.
- (< Fedora 27) Boost, Boost.Python: ``python3-devel``, and then see :ref:`install_boost_python`.
- Open Babel: ``openbabel-devel``.
- ``pdflatex``: ``texlive-scheme-full`` (less can do it, will be refined in the future).
- ``pdf2svg``: ``pdf2svg``.
- Graphviz: ``graphviz``.


.. _install_boost_python:

Boost and Boost.Python with Python 3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A package with the sources of Boost can be downloaded from `http://boost.org`.


Basic Installation
""""""""""""""""""

Boost uses a custom build system which may be difficult to use.
A procedure for compiling and installing Boost with Python 3 for Boost.Python is the following.

1. ``./bootstrap.sh --with-python=python3`` (optionally give ``--prefix=some/path`` to specify a non-standard installation path.
2. ``./b2`` (optionally give ``-j N`` (similar to GNU Make) to compile with multiple threads)
3. ``./b2 install`` (optionally give ``--prefix=some/path`` to specify a non-standard installation path (if not done earlier).

After ``bootstrap.sh`` has been run, the file ``project-config.jam``
has been created, which can be edited to customise installation path and a lot of other
things. All edits should be done before running ``b2``.


Non-standard Python Installation
""""""""""""""""""""""""""""""""

Passing ``--with-python=python3`` to ``bootstrap.sh`` should work.
This adds a line similar to "``using python : 3.3 ;``" to ``project-config.jam``.
After compilation (running ``b2``) the file ``stage/lib/libboost_python3.so`` should exist.
If not, it did not detect Python 3 properly.

If Python is installed in a non-standard location, add the a line similar to
"``using python : 3.3 : python3 : /path/to/python/3/installtion/include ;``" to
``project-config.jam``, where the last path is the path to the
``include``-folder of the Python-installation.


Custom GCC Version
""""""""""""""""""

Before running ``b2`` create the file ``user-config.jam`` in the root of the home dir (see
`here <http://www.boost.org/boost-build2/doc/html/bbv2/overview/configuration.html>`__
for the full documentation). Put a line similar to
"``using gcc : : /path/to/g++-4.8``" in the file.


.. _install_graphviz:

Graphviz
^^^^^^^^

A package with the sources of Graphviz can be downloaded from `http://graphviz.org`.
Make sure that the status output in the end of ``configure`` includes ``pangocairo:    Yes`` and ``rsvg:          Yes``.

