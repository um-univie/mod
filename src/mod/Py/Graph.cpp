#include <mod/Py/Common.h>

#include <mod/Chem.h>
#include <mod/Graph.h>

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
			// rst:
			// rst:			:param first: the printing options used for the first depiction.
			// rst:			:type first: :class:`GraphPrinter`
			// rst:			:param second: the printing options used for the second depiction.
			// rst:				If it is ``None`` then it is set to``first``.
			// rst:			:type second: :class:`GraphPrinter`
			// rst:			:returns: the names for the PDF-files that will be compiled in post-processing.
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




	// rst: .. py:class:: GraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a graph, or a null vertex.
	// rst:
	py::class_<mod::Graph::Vertex>("GraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::Graph::Vertex::getId)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The graph the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Graph`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("graph", &mod::Graph::Vertex::getGraph)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &mod::Graph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`IncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &mod::Graph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Graph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: :py:class:`AtomId`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &mod::Graph::Vertex::getAtomId)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: :py:class:`Charge`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &mod::Graph::Vertex::getCharge)
			;



	// rst: .. py:class:: GraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a graph, or a null edge.
	// rst:
	py::class_<mod::Graph::Edge>("GraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The graph the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Graph`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("graph", &mod::Graph::Edge::getGraph)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`GraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &mod::Graph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`GraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &mod::Graph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Graph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: :py:class:`BondType`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &mod::Graph::Edge::getBondType)
			;



	py::class_<mod::Graph::VertexRange>("GraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::Graph::VertexRange>())
			.def("__getitem__", &mod::Graph::VertexRange::operator[])
			;
	py::class_<mod::Graph::EdgeRange>("GraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Graph::EdgeRange>())
			;
	py::class_<mod::Graph::IncidentEdgeRange>("GraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Graph::IncidentEdgeRange>())
			;







	// rst: .. py:class:: GraphPrinter
	// rst:
	// rst:		This class is used to configure how graphs are visualised.
	// rst:
	// rst:		.. warning:: Some of these options greatly alter how graphs are depicted
	// rst:			and the result may not accurately represent the underlying graph,
	// rst:			and may make non-molecules look like molecules.
	// rst:
	py::class_<mod::GraphPrinter, boost::noncopyable>("GraphPrinter")
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			The default constructor enables edges as bonds and raised charges.
			// rst:		.. py:method:: setMolDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening and index printing.
			.def("setMolDefault", &mod::GraphPrinter::setMolDefault)
			// rst:		.. py:method:: setReactionDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening, index printing and simplification of carbon atoms.
			.def("setReactionDefault", &mod::GraphPrinter::setReactionDefault)
			// rst:		.. py:method:: disableAll()
			// rst:
			// rst:			Disable all special printing features.
			.def("disableAll", &mod::GraphPrinter::disableAll)
			// rst:		.. py:method:: enableAll()
			// rst:
			// rst:			Enable all special printing features, except typewriter font.
			.def("enableAll", &mod::GraphPrinter::enableAll)
			// rst:		.. py:attribute:: edgesAsBonds
			// rst:
			// rst:			Control whether edges with special labels are drawn as chemical bonds.
			// rst:
			// rst:			:type: bool
			.add_property("edgesAsBonds", &mod::GraphPrinter::getEdgesAsBonds, &mod::GraphPrinter::setEdgesAsBonds)
			// rst:		.. py:attribute:: collapseHydrogens
			// rst:
			// rst:			Control whether vertices representing hydrogen atoms are collapsed into their neighbours labels.
			// rst:
			// rst:			:type: bool
			.add_property("collapseHydrogens", &mod::GraphPrinter::getCollapseHydrogens, &mod::GraphPrinter::setCollapseHydrogens)
			// rst:		.. py:attribute:: raiseCharges
			// rst:
			// rst:			Control whether a vertex label suffix encoding a charge is written as a superscript to the rest of the label.
			// rst:
			// rst:			:type: bool
			.add_property("raiseCharges", &mod::GraphPrinter::getRaiseCharges, &mod::GraphPrinter::setRaiseCharges)
			// rst:		.. py:attribute:: simpleCarbons
			// rst:
			// rst:			Control whether some vertices encoding carbon atoms are depicted without any label.
			// rst:
			// rst:			:type: bool
			.add_property("simpleCarbons", &mod::GraphPrinter::getSimpleCarbons, &mod::GraphPrinter::setSimpleCarbons)
			// rst:		.. py:attribute:: thick
			// rst:
			// rst:			Control whether all edges are drawn thicker than normal and all labels are written in bold.
			// rst:
			// rst:			:type: bool
			.add_property("thick", &mod::GraphPrinter::getThick, &mod::GraphPrinter::setThick)
			// rst:		.. py:attribute:: withColour
			// rst:
			// rst:			Control whether colour is applied to certain elements of the graph which are molecule-like.
			// rst:
			// rst:			:type: bool
			.add_property("withColour", &mod::GraphPrinter::getWithColour, &mod::GraphPrinter::setWithColour)
			// rst:		.. py:attribute:: withIndex
			// rst:
			// rst:			Control whether the underlying indices of the vertices are printed.
			// rst:
			// rst:			:type: bool
			.add_property("withIndex", &mod::GraphPrinter::getWithIndex, &mod::GraphPrinter::setWithIndex)
			// rst:		.. py:attribute:: withTexttt
			// rst:
			// rst:			Control whether the vertex and edge labels are written with typewriter font.
			// rst:
			// rst:			:type: bool
			.add_property("withTexttt", &mod::GraphPrinter::getWithTexttt, &mod::GraphPrinter::setWithTexttt)
			;
}

} // namespace Py
} // namespace mod
