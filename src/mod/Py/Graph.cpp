#include <mod/Py/Common.h>

#include <mod/Graph.h>
#include <mod/GraphGraphInterface.h>
#include <mod/GraphPrinter.h>

#include <iomanip>

namespace mod {
namespace Py {

void Graph_doExport() {
	std::pair<std::string, std::string> (mod::Graph::*printWithoutOptions)() const = &mod::Graph::print;
	std::pair<std::string, std::string> (mod::Graph::*printWithOptions)(const mod::GraphPrinter&, const mod::GraphPrinter&) const = &mod::Graph::print;

	// rst: .. py:class:: Graph
	// rst:
	// rst:		This class models an undirected graph with labels on vertices and edges,
	// rst:		without loops and without parallel edges.
	// rst:		Certain labels are regarded as models of chemical atoms and bonds.
	// rst:		See :ref:`mol-enc` for more information on this.
	// rst:
	py::class_<mod::Graph, std::shared_ptr<mod::Graph>, boost::noncopyable>("Graph", py::no_init)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) A unique instance id among :class:`Graph` objects.
			// rst:	
			// rst:			:type: int
			.add_property("id", &mod::Graph::getId)
			.def(str(py::self))
			//------------------------------------------------------------------
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &mod::Graph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: :py:class:`GraphVertexRange`
			.add_property("vertices", &mod::Graph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &mod::Graph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: :py:class:`GraphEdgeRange`
			.add_property("edges", &mod::Graph::edges)
			//------------------------------------------------------------------
			// rst:		.. py:method:: print()
			// rst:		               print(first, second=None)
			// rst:
			// rst:			Print the graph, using either the default options or the options in ``first`` and ``second``.
			// rst:			If ``first`` and ``second`` are the same, only one depiction will be made.
			// rst:
			// rst:			:param first: the printing options used for the first depiction.
			// rst:			:type first: :class:`GraphPrinter`
			// rst:			:param second: the printing options used for the second depiction.
			// rst:				If it is ``None`` then it is set to ``first``.
			// rst:			:type second: :class:`GraphPrinter`
			// rst:			:returns: the names for the PDF-files that will be compiled in post-processing.
			// rst:				If ``first`` and ``second`` are the same, the two file prefixes are equal.
			// rst:			:rtype: (string, string)
			.def("print", printWithoutOptions)
			.def("print", printWithOptions)
			// rst:		.. py:method:: getGMLString(withCoords=False)
			// rst:
			// rst:			:returns: the :ref:`GML <graph-gml>` representation of the graph,
			// rst:			          optionally with generated 2D coordinates.
			// rst:			:rtype: string
			// rst:			:raises: :py:class:`LogicError` when coordinates are requested, but
			// rst:			         none can be generated.
			.def("getGMLString", &mod::Graph::getGMLString)
			// rst:		.. py:method:: printGML(withCoords=False)
			// rst:
			// rst:			Print the :ref:`GML <graph-gml>` representation of the graph,
			// rst:			optionally with generated 2D coordinates.
			// rst:
			// rst:			:returns: the filename of the printed GML file.
			// rst:			:rtype: string
			// rst:			:raises: :py:class:`LogicError` when coordinates are requested, but
			// rst:			         none can be generated.
			.def("printGML", &mod::Graph::printGML)
			// rst:		.. py:attribute:: name
			// rst:
			// rst:			The name of the graph. The default name includes the unique instance id.
			// rst:
			// rst:			:type: string
			.add_property("name", py::make_function(&mod::Graph::getName, py::return_value_policy<py::copy_const_reference>()),
			&mod::Graph::setName)
			// rst:		.. py:attribute:: smiles
			// rst:
			// rst:			(Read-only) If the graph models a molecule, this is the canonical :ref:`SMILES string <graph-smiles>` for it.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :any:`LogicError` if the graph is not a molecule.
			.add_property("smiles", py::make_function(&mod::Graph::getSmiles, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: graphDFS
			// rst:
			// rst:			(Read-only) This is the :ref:`GraphDFS <graph-graphDFS>` of the graph.
			// rst:
			// rst:			:type: string
			.add_property("graphDFS", py::make_function(&mod::Graph::getGraphDFS, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: linearEncoding
			// rst:
			// rst:			(Read-only) If the graph models a molecule this is the :ref:`SMILES string <graph-smiles>` string, otherwise it is the :ref:`GraphDFS <graph-graphDFS>` string.
			// rst:
			// rst:			:type: string
			.add_property("linearEncoding", py::make_function(&mod::Graph::getLinearEncoding, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: isMolecule
			// rst:
			// rst:			(Read-only) Whether or not the graph models a molecule. See :ref:`mol-enc`.
			// rst:
			// rst:			:type: bool
			.add_property("isMolecule", &mod::Graph::getIsMolecule)
			// rst:		.. py:attribute:: energy
			// rst:
			// rst:			(Read-only) If the graph models a molecule, this is some energy value.
			// rst:			The energy is calculated using Open Babel, unless already calculated or cached by :py:meth:`Graph.cacheEnergy`.
			// rst:
			// rst:			:type: double
			.add_property("energy", &mod::Graph::getEnergy)
			// rst:		.. py:method:: cacheEnergy(e)
			// rst:
			// rst:			If the graph models a molecule, sets the energy to a given value.
			// rst:
			// rst:			:param double e: the value for the energy to be set.
			.def("cacheEnergy", &mod::Graph::cacheEnergy)
			// rst:		.. py:attribute:: molarMass
			// rst:
			// rst:			(Read-only) If the graph models a molecule, this is the molar mass of the molecule as calculated by Open Babel.
			// rst:
			// rst:			:type: double
			.add_property("molarMass", &mod::Graph::getMolarMass)
			// rst:		.. py:method:: vLabelCount(label)
			// rst:
			// rst:			:param string label: some label for querying.
			// rst:			:returns: the number of vertices in the graph with the given label.
			// rst:			:rtype: unsigned int
			.def("vLabelCount", &mod::Graph::vLabelCount)
			// rst:		.. py:method:: eLabelCount(label)
			// rst:		
			// rst:			:param string label: some label for querying.
			// rst:			:returns: the number of edges in the graph with the given label.
			// rst:			:rtype: unsigned int
			.def("eLabelCount", &mod::Graph::eLabelCount)
			// rst:		.. py:method:: isomorphism(other, maxNumMatches=1)
			// rst:		
			// rst:			:param other: the codomain :class:`Graph` for finding morphisms.
			// rst:			:type other: :class:`Graph`
			// rst:			:param maxNumMatches: the maximum number of isomorphisms to search for.
			// rst:			:type maxNumMatches: :cpp:any:`std::size_t`
			// rst:			:returns: the number of isomorphisms from this graph to ``other``, but at most ``maxNumMatches``.
			// rst:			:rtype: :cpp:any:`std::size_t`
			.def("isomorphism", &mod::Graph::isomorphism)
			// rst:		.. py:method:: monomorphism(other, maxNumMatches=1)
			// rst:		
			// rst:			:param other: the codomain :class:`Graph` for finding morphisms.
			// rst:			:type host: :class:`Graph`
			// rst:			:param maxNumMatches: the maximum number of monomorphisms to search for.
			// rst:			:type maxNumMatches: :cpp:any:`std::size_t`
			// rst:			:returns: the number of monomorphisms from this graph to ``other``, though at most ``maxNumMatches``.
			// rst:			:rtype: :cpp:any:`std::size_t`
			.def("monomorphism", &mod::Graph::monomorphism)
			// rst:		.. py:method:: makePermutation()
			// rst:
			// rst:			:returns: a graph isomorphic to this, but with the vertex indices randomly permuted.
			// rst:			:rtype: :py:class:`Graph`
			.def("makePermutation", &mod::Graph::makePermutation)
			// rst:		.. py:attribute:: image
			// rst:
			// rst:			(Write-only) A custom depiction for the graph. The depiction file used will be the string
			// rst:			returned by the given function, with ``.pdf`` appended.
			// rst:			The function will only be called once.
			// rst:
			// rst:			:type: string()
			.add_property("image", &noGet, &mod::Graph::setImage)
			// rst:		.. py:attribute:: imageCommad
			// rst:
			// rst:			A command to be run in post-processing if a custom depiction is set.
			// rst:			The command is only run once.
			// rst:
			// rst:			:type: string
			.add_property("imageCommand", &mod::Graph::getImageCommand, &mod::Graph::setImageCommand)
			// rst:		.. py:method:: getVertexFromExternalId(id)
			// rst:
			// rst:			If the graph was not loaded from an external data format, then this function
			// rst:			always return a null descriptor.
			// rst:			If the graph was loaded from a SMILES string, but *any* class label was not unique,
			// rst:			then the function always return a null descriptor.
			// rst:
			// rst:			.. note:: In general there is no correlation between external and internal ids.
			// rst:
			// rst:			:param id: the external id to find the vertex descriptor for.
			// rst:			:type id: int
			// rst:			:returns: the vertex descriptor for the given external id.
			// rst:		            The descriptor is null if the external id was not used.
			// rst;			:rtype: :py:class:`Vertex`
			.def("getVertexFromExternalId", &mod::Graph::getVertexFromExternalId)
			;

	// rst: .. py:method:: graphGMLString(s, name=None)
	// rst:
	// rst:		Load a graph in :ref:`GML <graph-gml>` format from a given string.
	// rst:
	// rst:		:param string d: the string with the :ref:`GML <graph-gml>` data to load from.
	// rst:		:param string name: the name of the graph. If none is given the default name is used.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: :class:`Graph`
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphGMLString", &mod::Graph::graphGMLString);
	// rst: .. py:method:: graphGML(f, name=None)
	// rst:
	// rst:		Load a graph in :ref:`GML <graph-gml>` format from a given file.
	// rst:
	// rst:		:param string f: name of the :ref:`GML <graph-gml>` file to be loaded.
	// rst:		:param string name: the name of the graph. If none is given the default name is used.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: :class:`Graph`
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphGML", &mod::Graph::graphGML);
	// rst: .. py:method:: graphDFS(s, name=None)
	// rst:
	// rst:		Load a graph from a :ref:`GraphDFS <graph-graphDFS>` string.
	// rst:
	// rst:		:param string s: the :ref:`GraphDFS <graph-graphDFS>` string to parse.
	// rst:		:param string name: the name of the graph. If none is given the default name is used.
	// rst:		:returns: the loaded graph.
	// rst:		:rtype: :class:`Graph`
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("graphDFS", &mod::Graph::graphDFS);
	// rst: .. py:method:: smiles(s, name=None)
	// rst:
	// rst:		Load a molecule from a :ref:`SMILES <graph-smiles>` string.
	// rst:
	// rst:		:param string s: the :ref:`SMILES <graph-smiles>` string to parse.
	// rst:		:param string name: the name of the graph. If none is given the default name is used.
	// rst:		:returns: the loaded molecule.
	// rst:		:rtype: :class:`Graph`
	// rst:		:raises: :class:`InputError` on bad input.
	py::def("smiles", &mod::Graph::smiles);
}

} // namespace Py
} // namespace mod
