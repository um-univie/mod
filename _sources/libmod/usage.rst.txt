Compiling and Linking Client Code
=================================
The build system installs not only header files and the actual library, but
also a pkg-config file.
The following assumes that the path ``<install prefix>/lib/pkgconfig`` has been
added to the environment variable ``PKG_CONFIG_PATH``.

When compiling code using libMØD then the following scheme can be used:

.. code-block:: bash

    g++ $(pkg-config --cflags mod) <normal compilation arguments>

Linking can be done in the same way:

.. code-block:: bash

   g++ <normal linking arguments> $(pkg-config --libs mod)

And as a Makefile fragment (:download:`download <examples/using/Makefile>`):

.. literalinclude:: examples/using/Makefile
    :language: make
