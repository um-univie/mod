
.. _mod-wrapper:

****************************
The Wrapper Script (``mod``)
****************************

The script ``mod`` is a convenience script primarily for starting ``python3``
and after its exit execute PostMØD (the summary generator).
The execution can however be customised somewhat, e.g., by running ``python3``
thorugh ``valgrind`` or ``gdb``.
At some point the script will be able to call some other virtual machine than Python,
which is why the documentation will not refer directly to running ``python3`` but
instead simply say "running the selected virtual machine (VM)". As default ``python3``
is that VM.

The wrapper script does the following.

#. Clear the folder ``out/``. If it does not exist, if will be created.
   All printing functions in libMØD assumes ``out/`` is created.
#. Run the selected VM, optionally through some selected command (e.g., ``gdb``).
#. If the exit code of the VM is not 0, exit with that exit code.
#. If PostMØD is not selected to be run, exit with code 0.
#. Run PostMØD and exit with its exit code.


Usage
#####

.. program:: mod

.. code-block:: bash

  mod [options]

.. option:: --help, -h

  Shows the help output of the script.

.. option:: --version, -v

  Print the version of MØD and exit (the version is always printed in the start).

.. option:: --nopost

  Do not run PostMØD after the selected VM has executed.

.. option:: --profile

  Run the VM through `Valgrind <http://valgrind.org/>`_ with ``--tool=callgrind``.
  This option takes precedence over the ``memcheck`` option.

.. option:: --memcheck

  Run the VM through `Valgrind <http://valgrind.org/>`_ with standard options for
  memory check.
  If ``--debug`` is given as well, the options ``--vgdb=yes --vgdb-error=0`` are
  also given to ``valgrind`` such that ``gdb`` can be connected.

.. option:: --vgArgs <args>

  If either :option:`--profile` or :option:`--memcheck` is used, this inserts ``<args>`` as an extra
  argument for ``valgrind``.

.. option:: --debug

  Run the VM through `GDB <http://www.gnu.org/software/gdb/>`_.
  If :option:`--memcheck` is given as well, this is not the case, but GDB can then
  be connected to the process by the following steps:
  
  1. Run ``gdb python3`` (substitute ``python3`` for your VM).
  2. In the GDB prompt, run ``target remote | vgdb``
  3. In the GDB prompt, run ``continue``
  
.. option:: clean

  Do not run any VM or PostMØD, but remove the ``out`` and ``summary`` folders.

.. option:: -j <N>

  Give ``-j <N>`` to PostMØD (if executed). It will pass it on to ``make``.

.. option:: -f <filename>

  When ``python3`` is the VM, execute the code ``include("filename")``.

.. option:: -e <code>

  When ``python3`` is the VM, execute ``code`` (with a line break afterwards).

.. option:: -i

  Use interactive mode for the Python interpreter (i.e., pass ``-i`` as additional argument),
  but disable the log.
  If ``ipython3`` is available it will be used as interpreter, otherwise ``python3`` is used.

.. option:: -q

  Use quite mode. Certain messages are not printed.



Plugins (Loading of Additional Modules)
#######################################

It can be useful to always import additional modules when using MØD.
This can be achieved by writing a file in the following format::

  name: <module name>
  path: <Python import path>

When ``mod`` uses such a file, it will append ``<Python import path>`` to ``PYTHONPATH``,
and insert both ``import <module name>`` and ``from <module name> import *`` into the preamble.
The plugin specification file must be placed in a directory that is also mentioned in the
following environment variable.

.. envvar:: MOD_PLUGIN_PATH

  A colon separated list of paths to search for plugins. Non-directory paths are skipped.
  All files (non-recursively) in each specified directory are checked for plugin information.


PyMØD Preamble
##############

When the wrapper script is starting Python, it will execute a small preamble of
useful code before the user-defined code is executed.
The complete code, including the preamble, given to the Python interpereter can
be seen in ``out/input`` after execution of the wrapper script.
The preamble has the following effects.

* Setting ``RTLD_GLOBAL`` in the interpreters ``dlopen`` flags (see :ref:`creatingPyMODExt`)
* ``import mod`` and ``from mod import *``
* Defining the following function both in global scope and in the ``mod`` module.

  .. py:function:: include(fName, checkDup=True, putDup=True, skipDup=True) 
  
    Include the file ``fName`` in somewhat the same way that the ``#include`` directive
    in the C/C++ preprocessor includes files (e.g., handling relative paths).
    
    :param fName: the file to be included.
    :type fName: string
    :param checkDup: check if the file has already been included before (with ``putDup=True``)
    :type checkDup: bool
    :param putDup: remember that this file has been included in subsequent calls.
    :type putDup: bool
    :param skipDup: skip inclusion instead of exiting if the file has already been included
      before (with ``putDup=True``)
    :type skipDup: bool
