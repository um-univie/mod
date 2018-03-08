
.. py:currentmodule:: mod
.. cpp:namespace:: mod

.. _term-desc:

First-Order Terms
#################

Vertex/edge labels on graphs/rules can be interpreted either as text strings
or as `firt-order terms <https://en.wikipedia.org/wiki/Unification_(computer_science)#First-order_term>`__.
Additionally, for first-order terms there is a choice in which type of relation between terms
should be required in morphisms.
This can be controlled in each algorithm through label settings objects
(C++: :cpp:class:`LabelSettings`, Python: :py:class:`LabelSettings`).

A constant or function symbol is a word that can be matched
by the regex ``[A-Za-z0-9=#:.+-][A-Za-z0-9=#:.+-_]*``.
This means that all strings that are usually considered "molecular" can be reinterpreted
as constant symbols.

A variable symbol is a word that can be matched
by the regex ``_[A-Za-z0-9=#:.+-][A-Za-z0-9=#:.+-_]*``.
That is, variable is like a constant/function symbol, but with a ``_`` prepended.
An unnamed variable can be specified by the special wildcard symbol ``*``.

.. note:: Variable names matched by the regex ``_[HT][0-9][0-9]*`` may be generated
	when printing out graphs/rules. Any original variable names are not saved.

Function terms start with a function symbol followed by
a parenthesis with a comma-separated list of terms.
They may contain white-space.

If parsing of terms fails a specific exception is thrown
(C++: :cpp:class:`TermParsingError`, Python: :py:class:`TermParsingError`).
