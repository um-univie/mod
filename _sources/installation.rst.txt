Installation
============

The package is built and installed as a normal autotools project (i.e., use ``configure`` then ``make`` amd ``make install``).
The following is a list of dependencies, where relevant arguments for ``configure`` is shown in parenthesis.
Futher below are specific instructions on how to install these dependencies.

* This documentation requires `Sphinx`_, preferably in the newest development version (``--enable-doc-checks``, ``--disable-doc-checks``).
* libMØD:

  - A C++ compiler with reasonable C++14 support is needed. GCC 5.1 or later should work.
  - `Boost`_ dev, 1.59 or >= 1.61 (``--with-boost=<path>``).
  - (optional) `Open Babel`_ dev, >= 2.3.2 (``--with-OpenBabel=yes|no|<path>``).

* PyMØD (``--enable-pymod``, ``--disable-pymod``):

  - Python 3 dev
  - Boost.Python built with Python 3
  - (Optional) IPython 3

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

Install the following packages.

- (>= Ubuntu 16.10) Boost: ``libboost-regex-dev libboost-system-dev``.
- (>= Ubuntu 16.10) Boost.Python: ``python3-dev libboost-python-dev``.
- (< Ubuntu 16.10) Boost, Boost.Python: ``python3-dev``, and then see :ref:`install_boost_python`.
- (>= Ubuntu 14.04) Open Babel: ``libopenbabel-dev``.
- (< Ubuntu 14.04) Open Babel: install from source.
- ``pdflatex``: ``texlive-full`` (less can do it, will be refined in the future).
- ``pdf2svg``: ``pdf2svg``.
- Graphviz: The Ubuntu package unfortunately does not include the plugin for converting SVG to PDF via rsvg and cairo.
  ``librsvg2-dev libpango1.0-dev``, and see :ref:`install_graphviz`.


Fedora
^^^^^^

Install the following packages.

- (>= Fedora 24) Boost: ``boost-devel boost-regex boost-system``.
- (>= Fedora 24) Boost.Python: ``python3-devel boost-python3-devel``.
- (< Fedora 24) Boost, Boost.Python: ``python3-devel``, and then see :ref:`install_boost_python`.
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

