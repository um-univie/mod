Known Issues
============

Error ``<prefix>/share/mod/commonPreamble.fmt made by different executable version`` during post-processing
-----------------------------------------------------------------------------------------------------------

This error message may be printed by ``pdflatex`` when ``post_mod`` is compiling figures.

MØD installs a precompiled Latex format file for speeding up figure compilation.
However, if the Latex installation is upgraded after MØD is installed this error may happen.

Completely reinstalling MØD will definitely solve the problem.
However, in the build directory, if you find ``commonPreabmle.fmt`` and delete it,
then running ``make`` and ``make install`` should quickly recompile just the format file
and update the installation.
