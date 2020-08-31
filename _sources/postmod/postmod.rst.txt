****************************
PostMØD (``mod_post``)
****************************

The post processor, ``mod_post``, is used for compiling figures and a summary document
of the data in the ``out/`` folder. As default it is automatically invoked by the
wrapper script.

The script does the following:

#. Clear the folder ``summary``. If it does not exist, it is created.
#. Source the file ``out/post.sh`` and create ``summary/Makefile``.
#. Run ``make -f summary/Makefile all`` (with a few more arguments).
   The Makefile will compile figures and generate a Latex file.
   A master Latex docuemnt is compiled to ``summary/summary.pdf``.
   The master document includes the generated Latex file.


Usage
#####

.. program:: mod_post

.. code-block:: bash

  mod_post [options]


Options
-------

.. option:: --help, -h

  Shows the help output of the script.

.. option:: -j <N>

  When running ``make``, use ``-j <N>`` as additional arguments.
  This parameter defaults to 2.


Environtment Variables
----------------------

.. envvar:: MOD_NUM_POST_THREADS

  If defined, the value of this variable is used for ``-j`` when invoking ``make``.
  When ``-j <N>`` is given to the script, this variable is ignored.


Useful API References
#####################

The API of libMØD and PyMØD includes functionality for injecting code and modifying the
behaviour of PostMØD and generated summaries. See :doc:`libMØD (Misc.h) </libmod/Misc>`
and :doc:`PyMØD (Misc) </pymod/Misc>` for the documentation of these functions.

