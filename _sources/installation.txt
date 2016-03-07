Installation
============

The package is built and installed as a normal autotools project (i.e., use ``configure`` then ``make`` amd ``make install``).
The following is a list of dependencies, where relevant arguments for ``configure`` is shown in parenthesis.
Futher below are specific instructions on how to install these dependencies.

* This documentation requires `Sphinx`_, preferably in the newest development version (``--enable-doc-checks``, ``--disable-doc-checks``).
* libMØD:

  - A C++ compiler with reasonable C++11 support is needed. GCC 4.7 or later should work.
  - `Boost`_ dev, >= 1.56 (``--with-boost=<path>``).
  - (optional) `Open Babel`_ dev, >= 2.3.2 (``--with-OpenBabel=yes|no|<path>``).
  - (optional, default off) `libSBML`_ (``--with-SBML=yes|no|<path>``).

* PyMØD (``--enable-pymod``, ``--disable-pymod``):

  - Python 3 dev
  - Boost.Python built with Python 3

* PostMØD (``--enable-postmod``, ``--disable-postmod``):

  - ``pdflatex`` available in the ``PATH``, with not too old packages (Tex Live 2012 or newer should work).
  - ``pdf2svg`` available in the ``PATH``
  - ``dot`` and ``neato`` from Graphviz available in the ``PATH``.
    They must additionally be able to load SVG files and output as both SVG and PDF files (all via cairo).
    That is, in the output of ``dot -P`` the following edges must exist:

    - ``cairo_device_svg -> output_svg``
    - ``cairo_device_pdf -> output_pdf``
    - ``rsvg_loadimage_svg -> render_cairo``


Ubuntu
^^^^^^

- (>= Ubuntu 15.10) Boost: ``libboost-regex-dev libboost-system-dev``.
- (>= Ubuntu 15.10) Boost.Python: ``python3-dev libboost-python-dev``.
- (< Ubuntu 15.10) Boost, Boost.Python: ``python3-dev``, and see :ref:`install_boost_python`.
- (>= Ubuntu 14.04) Open Babel: ``libopenbabel-dev``.
- (< Ubuntu 14.04) Open Babel: install from source.
- ``pdflatex``: ``texlive-full`` (less can do it, will be refined in the future).
- ``pdf2svg``: ``pdf2svg``.
- Graphviz: The Ubuntu package unfortunately does not include the plugin for converting SVG to PDF via rsvg and cairo.
  ``librsvg2-dev libpango1.0-dev``, and see :ref:`install_graphviz`.


Fedora
^^^^^^

- Boost: ``boost-devel boost-regex boost-system``.
- Boost.Python: ``python3-devel boost-python3-devel``.
- Boost, Boost.Python: ``python3-devel``, and see :ref:`install_boost_python`.
- Open Babel: ``openbabel-devel``.
- ``pdflatex``: ``texlive-scheme-full`` (less can do it, will be refined in the future).
- ``pdf2svg``: ``pdf2svg``.
- Graphviz: ``graphviz``.


Common
^^^^^^

- libSBML: install from source, see :ref:`install_libsbml`.


.. _install_boost_python:

Boost and Boost.Python with Python 3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A package with the sources of Boost can be downloaded from `http://boost.org`.

Basic Installation
""""""""""""""""""

Boost uses a custom build system which may be difficult to use.
The general procedure for compiling and installing Bosot is the following.

1. ``./bootstrap.sh`` (optionally give ``--prefix=some/path`` to specify a non-standard installation path.
2. ``./b2`` (optionally give ``-j N`` (similar to GNU Make) to compile with multiple threads)
3. ``./b2 install`` (optionally give ``--prefix=some/path`` to specify a non-standard installation path (if not done earlier).

After ``bootstrap.sh`` has been run, the file ``project-config.jam``
has been created, which can be edited to customise installation path and a lot of other
things. All edits should be done before running ``b2``.

Custom Python Version (e.g., Python 3 for Boost.Python)
"""""""""""""""""""""""""""""""""""""""""""""""""""""""

This can be achieved in multiple ways. If the custom version (here assumed to be 3.3) is
installed in the normal system location, then passing ``--with-python=python3`` to
``bootstrap.sh`` should work. This adds a line similar to
"``using python : 3.3 ;``" to ``project-config.jam``. After compilation
(running ``b2``) the file ``stage/lib/libboost_python3.so`` should exist.
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


.. _install_libsbml:

libSBML
^^^^^^^

Note that at version 5.8 (and probably other versions too) the library is
transitioning from using Autotools to CMake and both build systems coexist.
The following assumes CMake is used.

The option ``-DWITH_CPP_NAMESPACE=yes`` **must** be used for configuration.
This will wrap the complete library in the C++ namespace ``libsbml``, which is
currently not the default behaviour.

(The following should not be needed, but if libMØD compilation fails with
libSBML related errors, then try it)
In version 5.8 (and maybe other versions), the build system does not
install the correct header files when namespace wrapping is enabled.
After installation, open the file

.. code-block:: bash

    <installation prefix>/include/sbml/common/libsbml-namespace.h

Change line 42 from

.. code-block:: c++

    /* #undef LIBSBML_USE_CPP_NAMESPACE \*/

to

.. code-block:: c++

    #define LIBSBML_USE_CPP_NAMESPACE

