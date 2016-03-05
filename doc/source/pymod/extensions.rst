
.. _creatingPyMODExt:

Creating a Python Extension
===========================

PyMØD is primarily Python bindings for libMØD made using
`Boost.Python <http://www.boost.org/libs/python/>`_.
The following shows a template for creating a Python extension with C++
functions (e.g., to extend libMØD).

Library Source and Headers
--------------------------

For this examples the following C++ source with header should be exposed
in the Python module. Header (:download:`download<examples/extension/stuff.h>`):

.. literalinclude:: examples/extension/stuff.h
    :language: cpp

Source (:download:`download<examples/extension/stuff.cpp>`):

.. literalinclude:: examples/extension/stuff.cpp
    :language: cpp

Exposing the Interface
----------------------

See the Boost.Python documentation for instructions how to expose C++ code.
Below is the code for creating our simple Python extension
(:download:`download<examples/extension/pyModule.cpp>`).

.. literalinclude:: examples/extension/pyModule.cpp
    :language: cpp

The example exposes a bit of extra functionality for a sanity check.
Python will ``dlopen`` libMØD twice As both the PyMØD module and our extension
requires it. The library uses static variables and strange things might happen
if multiple instances of these exist. The MØD wrapper script changes the dlopen
flags (setting ``RTLD_GLOBAL`` and ``RTLD_NOW``) which should prevent multiple
instances of the library. The function ``magicLibraryValue`` can be used to check
that this is true (see the test script below).

Makefile
--------

A Makefile fragment to build the example (:download:`download<examples/extension/Makefile>`):

.. literalinclude:: examples/extension/Makefile
    :language: make

Testing the Extension
---------------------

Using the Makefile above creates the shared library ``awesome.so`` which contains
the extension. Executing the following script using the wrapper script in the same folder
as the shared libray should now work (:download:`download<examples/extension/test.py>`):

.. literalinclude:: examples/extension/test.py
    :language: py

Command to execute:

.. code-block:: sh

    mod -f test.py
