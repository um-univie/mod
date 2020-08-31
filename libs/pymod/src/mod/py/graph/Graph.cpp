#include <mod/py/Common.hpp>

#include <mod/graph/Graph.hpp>
#include <mod/graph/Automorphism.hpp>
#include <mod/graph/GraphInterface.hpp>
#include <mod/graph/Printer.hpp>

#include <iomanip>

namespace mod {
namespace graph {
namespace Py {

void Graph_doExport() {
	std::pair<std::string, std::string>(Graph::*
	printWithoutOptions)() const = &Graph::print;
	std::pair<std::string, std::string>(Graph::*
	printWithOptions)(const graph::Printer&, const graph::Printer&) const = &Graph::print;

	// rst: .. py:class:: Graph
	// rst:
	// rst:		This class models an undirected graph with labels on vertices and edges,
	// rst:		without loops and without parallel edges.
	// rst:		Certain labels are regarded as models of chemical atoms and bonds.
	// rst:		See :ref:`mol-enc` for more information on this.
	// rst:
	py::class_<Graph, std::shared_ptr<Graph>, boost::noncopyable>("Graph", py::no_init)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) A unique instance id among :class:`Graph` objects.
			// rst:	
			// rst:			:type: int
			.add_property("id", &Graph::getId)
			.def(str(py::self))
					//------------------------------------------------------------------
					// rst:		.. py:attribute:: numVertices
					// rst:
					// rst:			(Read-only) The number of vertices in the graph.
					// rst:
					// rst:			:type: int
			.add_property("numVertices", &Graph::numVertices)
					// rst:		.. py:attribute:: vertices
					// rst:
					// rst:			(Read-only) An iterable of all vertices in the graph.
					// rst:
					// rst:			:type: GraphVertexRange
			.add_property("vertices", &Graph::vertices)
					// rst:		.. py:attribute:: numEdges
					// rst:
					// rst:			(Read-only) The number of edges in the graph.
					// rst:
					// rst:			:type: int
			.add_property("numEdges", &Graph::numEdges)
					// rst:		.. py:attribute:: edges
					// rst:
					// rst:			(Read-only) An iterable of all edges in the graph.
					// rst:
					// rst:			:type: GraphEdgeRange
			.add_property("edges", &Graph::edges)
					//------------------------------------------------------------------
					// rst:		.. py:function:: aut(labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism))
					// rst:
					// rst:			:param LabelSettings labelSettings: the label settings to use.
					// rst:			:returns: an object representing the automorphism group of the graph, with the given label settings.
					// rst:			:rtype: GraphAutGroup
			.def("aut", &Graph::aut)
					//------------------------------------------------------------------
					// rst:		.. py:method:: print()
					// rst:		               print(first, second=None)
					// rst:
					// rst:			Print the graph, using either the default options or the options in ``first`` and ``second``.
					// rst:			If ``first`` and ``second`` are the same, only one depiction will be made.
					// rst:
					// rst:			:param GraphPrinter first: the printing options used for the first depiction.
					// rst:			:param GraphPrinter second: the printing options used for the second depiction.
					// rst:				If it is ``None`` then it is set to ``first``.
					// rst:			:returns: the names for the PDF-files that will be compiled in post-processing.
					// rst:				If ``first`` and ``second`` are the same, the two file prefixes are equal.
					// rst:			:rtype: tuple[str, str]
			.def("print", printWithoutOptions)
			.def("print", printWithOptions)
					// rst:		.. py:method:: printTermState
					// rst:
					// rst:			Print the term state for the graph.
			.def("printTermState", &Graph::printTermState)
					// rst:		.. py:method:: getGMLString(withCoords=False)
					// rst:
					// rst:			:returns: the :ref:`GML <graph-gml>` representation of the graph,
					// rst:			          optionally with generated 2D coordinates.
					// rst:			:rtype: str
					// rst:			:raises: :py:class:`LogicError` when coordinates are requested, but
					// rst:			         none can be generated.
			.def("getGMLString", &Graph::getGMLString)
					// rst:		.. py:method:: printGML(withCoords=False)
					// rst:
					// rst:			Print the :ref:`GML <graph-gml>` representation of the graph,
					// rst:			optionally with generated 2D coordinates.
					// rst:
					// rst:			:returns: the filename of the printed GML file.
					// rst:			:rtype: str
					// rst:			:raises: :py:class:`LogicError` when coordinates are requested, but
					// rst:			         none can be generated.
			.def("printGML", &Graph::printGML)
					// rst:		.. py:attribute:: name
					// rst:
					// rst:			The name of the graph. The default name includes the unique instance id.
					// rst:
					// rst:			:type: str
			.add_property("name", py::make_function(&Graph::getName, py::return_value_policy<py::copy_const_reference>()),
			              &Graph::setName)
					// rst:		.. py:attribute:: smiles
					// rst:
					// rst:			(Read-only) If the graph models a molecule, this is the canonical :ref:`SMILES string <graph-smiles>` for it.
					// rst:
					// rst:			:type: str
					// rst:			:raises: :py:class:`LogicError` if the graph is not a molecule.
			.add_property("smiles",
			              py::make_function(&Graph::getSmiles, py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. py:attribute:: smilesWithIds
					// rst:
					// rst:			(Read-only) If the graph models a molecule, this is the canonical :ref:`SMILES string <graph-smiles>` for it,
					// rst:			that includes the internal vertex id as a class label on each atom.
					// rst:
					// rst:			:type: str
					// rst:			:raises: :py:class:`LogicError` if the graph is not a molecule.
			.add_property("smilesWithIds",
			              py::make_function(&Graph::getSmilesWithIds, py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. py:attribute:: graphDFS
					// rst:
					// rst:			(Read-only) This is a :ref:`GraphDFS <graph-graphDFS>` of the graph.
					// rst:
					// rst:			:type: str
			.add_property("graphDFS",
			              py::make_function(&Graph::getGraphDFS, py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. py:attribute:: graphDFSWithIds
					// rst:
					// rst:			(Read-only) This is a :ref:`GraphDFS <graph-graphDFS>` of the graph, where each vertices have an explicit id,
					// rst:			corresponding to its internal vertex id.
					// rst:
					// rst:			:type: str
			.add_property("graphDFSWithIds", py::make_function(&Graph::getGraphDFSWithIds,
			                                                   py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. py:attribute:: linearEncoding
					// rst:
					// rst:			(Read-only) If the graph models a molecule this is the :ref:`SMILES string <graph-smiles>` string, otherwise it is the :ref:`GraphDFS <graph-graphDFS>` string.
					// rst:
					// rst:			:type: str
			.add_property("linearEncoding", py::make_function(&Graph::getLinearEncoding,
			                                                  py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. py:attribute:: isMolecule
					// rst:
					// rst:			(Read-only) Whether or not the graph models a molecule. See :ref:`mol-enc`.
					// rst:
					// rst:			:type: bool
			.add_property("isMolecule", &Graph::getIsMolecule)
					// rst:		.. py:attribute:: energy
					// rst:
					// rst:			(Read-only) If the graph models a molecule, this is some energy value.
					// rst:			The energy is calculated using Open Babel, unless already calculated or cached by :py:meth:`Graph.cacheEnergy`.
					// rst:
					// rst:			:type: float
			.add_property("energy", &Graph::getEnergy)
					// rst:		.. py:method:: cacheEnergy(e)
					// rst:
					// rst:			If the graph models a molecule, sets the energy to a given value.
					// rst:
					// rst:			:param float e: the value for the energy to be set.
			.def("cacheEnergy", &Graph::cacheEnergy)
					// rst:		.. py:attribute:: exactMass
					// rst:
					// rst:			(Read-only) The exact mass of the graph, if it is a molecule.
					// rst:			It is the sum of the exact mass of each atom, with the mass of electrons subtracted corresponding to the integer charge.
					// rst:			That is, the mass is :math:`\sum_a (mass(a) - mass(e)\cdot charge(a))`.
					// rst:			If an atom has no specified isotope, then the most abundant is used.
					// rst:
					// rst:			:type: float
					// rst:			:raises: :class:`LogicError` if it is not a molecule, including if some isotope has not been tabulated.
			.add_property("exactMass", &Graph::getExactMass)
					// rst:		.. py:method:: vLabelCount(label)
					// rst:
					// rst:			:param str label: some label for querying.
					// rst:			:returns: the number of vertices in the graph with the given label.
					// rst:			:rtype: int
			.def("vLabelCount", &Graph::vLabelCount)
					// rst:		.. py:method:: eLabelCount(label)
					// rst:
					// rst:			:param str label: some label for querying.
					// rst:			:returns: the number of edges in the graph with the given label.
					// rst:			:rtype: int
			.def("eLabelCount", &Graph::eLabelCount)
					// rst:		.. py:method:: isomorphism(other, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism))
					// rst:
					// rst:			:param Graph other: the codomain :class:`Graph` for finding morphisms.
					// rst:			:param int maxNumMatches: the maximum number of isomorphisms to search for.
					// rst:			:param LabelSettings labelSettings: the label settings to use during the search.
					// rst:			:returns: the number of isomorphisms from this graph to ``other``, but at most ``maxNumMatches``.
					// rst:			:rtype: int
			.def("isomorphism", &Graph::isomorphism)
					// rst:		.. py:method:: monomorphism(other, maxNumMatches=1, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism))
					// rst:
					// rst:			:param Graph other: the codomain :class:`Graph` for finding morphisms.
					// rst:			:param int maxNumMatches: the maximum number of monomorphisms to search for.
					// rst:			:param LabelSettings labelSettings: the label settings to use during the search.
					// rst:			:returns: the number of monomorphisms from this graph to ``other``, though at most ``maxNumMatches``.
					// rst:			:rtype: int
			.def("monomorphism", &Graph::monomorphism)
					// rst:		.. py:method:: makePermutation()
					// rst:
					// rst:			:returns: a graph isomorphic to this, but with the vertex indices randomly permuted.
					// rst:			:rtype: Graph
			.def("makePermutation", &Graph::makePermutation)
					// rst:		.. py:attribute:: image
					// rst:
					// rst:			(Write-only) A custom depiction for the graph. The depiction file used will be the string
					// rst:			returned by the given function, with ``.pdf`` appended.
					// rst:			The function will only be called once.
					// rst:
					// rst:			:type: Callable[[], str]
			.add_property("image", &mod::Py::noGet, &Graph::setImage)
					// rst:		.. py:attribute:: imageCommad
					// rst:
					// rst:			A command to be run in post-processing if a custom depiction is set.
					// rst:			The command is only run once.
					// rst:
					// rst:			:type: str
			.add_property("imageCommand", &Graph::getImageCommand, &Graph::setImageCommand)
					// rst:		.. py:method:: getVertexFromExternalId(id)
					// rst:
					// rst:			If the graph was not loaded from an external data format, then this function
					// rst:			always return a null descriptor.
					// rst:			If the graph was loaded from a SMILES string, but *any* class label was not unique,
					// rst:			then the function always return a null descriptor.
					// rst:
					// rst:			.. note:: In general there is no correlation between external and internal ids.
					// rst:
					// rst:			:param int id: the external id to find the vertex descriptor for.
					// rst:			:returns: the vertex descriptor for the given external id.
					// rst:		            The descriptor is null if the external id was not used.
					// rst:			:rtype: GraphVertex
			.def("getVertexFromExternalId", &Graph::getVertexFromExternalId)
					// rst:		.. py:attribute:: minExternalId
					// rst:		                  maxExternalId
					// rst:
					// rst:			If the graph was not loaded from an external data format, then these attributes
					// rst:			are always return 0. Otherwise, they are the minimum/maximum external id from which
					// rst:			non-null vertices can be obtained from :py:meth:`getVertexFromExternalId`.
					// rst:			If no such minimum and maximum exists, then they are 0.
					// rst:
					// rst:			:type: int
			.add_property("minExternalId", &Graph::getMinExternalId)
			.add_property("maxExternalId", &Graph::getMaxExternalId)
					// rst: 	.. py:method:: instantiateStereo()
					// rst:
					// rst: 		Make sure that stereo data is instantiated.
					// rst:
					// rst: 		:raises: :py:class:`StereoDeductionError` if the data was not instantiated and deduction failed.
			.def("instantiateStereo", &Graph::instantiateStereo);

	// rst: .. py:data:: inputGraphs
	// rst:
	// rst:		A list of graphs to which explicitly loaded graphs as default are appended.
	// rst:
	// rst:		:type: list[Graph]
	// rst:

	// rst: .. py:method:: graphGMLString(s, name=None, add=True)
	// rst:
	// rst:		Load a graph in :ref:`GML <graph-gml>` format from a given string.
	// rst:
	// rst:		:param str s: the string with the :ref:`GML <graph-gml>` data to load from.
	// rst:		:param str name: the name of the graph. If none is given the default name is used.
	// rst:		:param bool add: whether to append the graph to :data:`inputGraphs` or not.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: Graph
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphGMLString", &Graph::graphGMLString);
	// rst: .. py:method:: graphGML(f, name=None, add=True)
	// rst:
	// rst:		Load a graph in :ref:`GML <graph-gml>` format from a given file.
	// rst:
	// rst:		:param str f: name of the :ref:`GML <graph-gml>` file to be loaded.
	// rst:		:param str name: the name of the graph. If none is given the default name is used.
	// rst:		:param bool add: whether to append the graph to :data:`inputGraphs` or not.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: Graph
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphGML", &Graph::graphGML);
	// rst: .. py:method:: graphDFS(s, name=None, add=True)
	// rst:
	// rst:		Load a graph from a :ref:`GraphDFS <graph-graphDFS>` string.
	// rst:
	// rst:		:param str s: the :ref:`GraphDFS <graph-graphDFS>` string to parse.
	// rst:		:param str name: the name of the graph. If none is given the default name is used.
	// rst:		:param bool add: whether to append the graph to :data:`inputGraphs` or not.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: Graph
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphDFS", &Graph::graphDFS);
	// rst: .. py:method:: smiles(s, name=None, add=True, allowAbstract=False, classPolicy=SmilesClassPolicy.NoneOnDuplicate)
	// rst:
	// rst:		Load a molecule from a :ref:`SMILES <graph-smiles>` string.
	// rst:
	// rst:		:param str s: the :ref:`SMILES <graph-smiles>` string to parse.
	// rst:		:param str name: the name of the graph. If none is given the default name is used.
	// rst:		:param bool add: whether to append the graph to :data:`inputGraphs` or not.
	// rst:		:param bool allowAbstract: whether to allow abstract vertex labels in bracketed atoms.
	// rst:		:returns: the loaded molecule.
	// rst:		:rtype: Graph
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("smiles",
	        static_cast<std::shared_ptr<Graph> (*)(const std::string &, bool, SmilesClassPolicy)>(&Graph::smiles));
}

} // namespace Py
} // namespace graph
} // namespace mod
