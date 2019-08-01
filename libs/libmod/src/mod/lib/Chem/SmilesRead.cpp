// see http://www.opensmiles.org/opensmiles.html

//#define BOOST_SPIRIT_X3_DEBUG
//#include <boost/optional/optional_io.hpp>

#include "Smiles.hpp"

#include <mod/Config.hpp>
#include <mod/Error.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/ParsingUtil.hpp>
#include <mod/lib/Stereo/Inference.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eoi.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/not_predicate.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

namespace mod {
namespace lib {
namespace Chem {
namespace Smiles {
using Vertex = lib::Graph::Vertex;
using Edge = lib::Graph::Edge;

struct Charge {

	Charge() : charge(0) { }

	Charge(std::pair<char, int> data) : charge(data.second) {
		switch(data.first) {
		case 0:
			charge = 0;
			break;
		case '-':
			charge *= -1;
			break;
		case '+':
			break;
		default:
			MOD_ABORT;
		}
	}

	operator int() const {
		return charge;
	}

	friend std::ostream &operator<<(std::ostream &s, Charge c) {
		if(c.charge != 0) {
			s << (c.charge < 0 ? '-' : '+');
			if(std::abs(c.charge) > 1) s << std::abs(c.charge);
		}
		return s;
	}
public:
	int charge;
};

struct Chiral {
	std::string specifier;
	unsigned int k;
private:

	Chiral(std::string specifier, unsigned int k) : specifier(specifier), k(k) { }
public:
	Chiral() = default;

	static Chiral makeSimple(std::string specifier) {
		return Chiral(specifier, 0);
	}

	static Chiral makeTB(unsigned int k) {
		return Chiral("TB", k);
	}

	static Chiral makeOH(unsigned int k) {
		return Chiral("OH", k);
	}

	friend std::ostream &operator<<(std::ostream &s, const Chiral &ch) {
		if(!ch.specifier.empty()) {
			s << ch.specifier;
			if(ch.specifier == "@TB" || ch.specifier == "@OH") s << ch.k;
		}
		return s;
	}
};

struct Atom {
	unsigned int isotope = 0;
	std::string symbol;
	bool isImplicit = false;
	Chiral chiral;
	unsigned int hCount = 0;
	Charge charge;
	bool radical;
	int class_ = -1;
public:
	bool isAromatic;
	Vertex vertex;
	AtomId atomId;
	std::vector<Vertex> hydrogens;
public:
	Atom() = default;

	Atom(const std::string &shorthandSymbol) : symbol(shorthandSymbol), isImplicit(true) { }

	friend std::ostream &operator<<(std::ostream &s, const Atom &a) {
		if(!a.isImplicit) {
			s << "[";
			if(a.isotope > 0) s << a.isotope;
		}
		s << a.symbol;
		if(!a.isImplicit) {
			s << a.chiral;
			if(a.hCount > 0) {
				s << "H";
				if(a.hCount > 1) s << a.hCount;
			}
			s << a.charge;
			if(a.radical) s << '.';
			if(a.class_ != -1) s << ":" << a.class_;
			s << "]";
		}
		return s;
	}
};

struct BranchedAtom;
std::ostream &operator<<(std::ostream &s, const BranchedAtom &ba);

struct BondBranchedAtomPair {
	unsigned int bond;
	x3::forward_ast<BranchedAtom> atom;
public:
	Edge edge;

	friend std::ostream &operator<<(std::ostream &s, const BondBranchedAtomPair &bba) {
		if(bba.bond != 0) s << char(bba.bond);
		return s << bba.atom.get();
	}
};

struct ChainTail {
	std::vector<BondBranchedAtomPair> elements;
	int dummy;
public:

	friend std::ostream &operator<<(std::ostream &s, const ChainTail &ct) {
		for(auto &&e : ct.elements) s << e;
		return s;
	}
};

struct RingBond {
	unsigned int bond;
	unsigned int ringId;
public:
	Edge edge;

	friend std::ostream &operator<<(std::ostream &s, const RingBond &rb) {
		if(rb.bond != 0) s << char(rb.bond);
		if(rb.ringId > 9) s << "%";
		return s << rb.ringId;
	}
};

struct BranchedAtom {
	Atom atom;
	std::vector<RingBond> ringBonds;
	std::vector<ChainTail> branches;

	friend std::ostream &operator<<(std::ostream &s, const BranchedAtom &ba) {
		s << ba.atom;
		for(const auto &rb : ba.ringBonds) s << rb;
		for(const auto &b : ba.branches) s << '(' << b << ')';
		return s;
	}

	friend std::ostream &operator<<(std::ostream &s, const x3::forward_ast<BranchedAtom> &ba) {
		if(auto *p = ba.get_pointer()) return s << *p;
		else return s << "NullBranchedAtom";
	}
};

struct SmilesChain {
	BranchedAtom branchedAtom;
	ChainTail tail;

	friend std::ostream &operator<<(std::ostream &s, const SmilesChain &c) {
		return s << c.branchedAtom << c.tail;
	}
};

namespace {
namespace parser {

struct BondSymbol : x3::symbols<unsigned int> {

	BondSymbol() {
		name("bondSymbol");
		for(char c :{'-', '=', '#', '$', ':', '/', '\\'})
			add(std::string(1, c), c);
	}
} bondSymbol;

struct ShorthandSymbol : x3::symbols<Atom> {

	ShorthandSymbol() {
		name("aliphaticOrganic|aromaticOrganic|wildcardSymbol");
		for(auto atomId : getSmilesOrganicSubset())
			add(symbolFromAtomId(atomId), symbolFromAtomId(atomId));
		for(std::string str :{"b", "c", "n", "o", "s", "p"})
			add(str, str);
		add("*", std::string("*"));
	}

} shorthandSymbol;

struct ElementSymbol : x3::symbols<std::string> {

	ElementSymbol() {
		name("elementSymbol");
		for(unsigned char i = 1; i <= AtomIds::Max; i++)
			add(symbolFromAtomId(AtomId(i)), symbolFromAtomId(AtomId(i)));
	}
} elementSymbol;

struct AromaticSymbol : x3::symbols<std::string> {

	AromaticSymbol() {
		name("aromaticSymbol");
		for(std::string str :{"b", "c", "n", "o", "p", "s", "se", "as"}) add(str, str);
	}
} aromaticSymbol;

struct ChiralSymbl : x3::symbols<Chiral> {

	ChiralSymbl() {
		name("chiralSymbol");
		for(std::string str :{"@", "@@", "@TH1", "@TH2", "@AL1", "@AL2", "@SP1", "@SP2", "@SP3"})
			add(str, Chiral::makeSimple(str));
		for(unsigned int i = 1; i < 20; i++)
			add("@TB" + boost::lexical_cast<std::string>(i), Chiral::makeTB(i));
		for(unsigned int i = 1; i < 30; i++)
			add("@OH" + boost::lexical_cast<std::string>(i), Chiral::makeOH(i));
	}
} chiralSymbol;

// no recursion
const x3::rule<struct isotope, int> isotope = "isotope (not 0)";

const auto singleDigit = x3::uint_parser<int, 10, 1, 1>();
const auto doubleDigit = x3::uint_parser<int, 10, 1, 2>();
const auto class_ = (x3::lit(':') > x3::uint_) | x3::attr(-1);
const auto radical = (x3::lit('.') >> x3::attr(true)) | x3::attr(false);
const auto chargeNum = doubleDigit | x3::attr(1);
const auto charge = x3::rule<struct charge, std::pair<char, int> >{"charge"}
/*             */ = (x3::char_("+-") | x3::attr(0)) >> chargeNum;
const auto hcount = (x3::lit('H') > (singleDigit | x3::attr(1))) // singleDigit may NOT parse unary + or -
/*             */ | x3::attr(0);
const auto chiral = chiralSymbol | x3::attr(Chiral());
const auto symbol = elementSymbol | aromaticSymbol | x3::string("*");
const auto isotope_def = !x3::lit('0') >> (x3::uint_ | x3::attr(0));
const auto bracketAtom = x3::rule<struct bracketAtom, Atom>{"bracketAtom"}
/*                  */ = x3::lit('[') > isotope > symbol > chiral > hcount > charge > radical > class_ > ']';
const auto atom = bracketAtom | shorthandSymbol;
const auto ringId = singleDigit | (x3::lit('%') > doubleDigit);
const auto ringBond = x3::rule<struct ringBond, RingBond>{"ringBond"}
/*               */ = (bondSymbol | x3::attr(0)) >> ringId;
const auto ringBonds = *ringBond;
const auto bond = bondSymbol | x3::char_('.') | x3::attr(0);

// part of recursion
const x3::rule<struct chainTail, ChainTail> chainTail = "chainTail";
const auto branch = x3::rule<struct branch, ChainTail>{"branch"}
/*             */ = x3::lit('(') > chainTail > ')';
const auto branches = *branch;
const auto branchedAtom = x3::rule<struct branchedAtom, BranchedAtom>{"branchedAtom"}
/*                   */ = atom > ringBonds > branches;
const auto bondBranchedAtomPair = x3::rule<struct bondBranchedAtomPair, BondBranchedAtomPair>{"bondBranchedAtomPair"}
/*                           */ = bond >> branchedAtom;
const auto chainTail_def = *bondBranchedAtomPair >> x3::attr(0); // dummy attr(0) because of a quirk in the attribute collapsing
BOOST_SPIRIT_DEFINE(chainTail, isotope);

// no recursion
const auto smiles = x3::rule<struct smiles, SmilesChain>{"smiles"}
/*             */ = branchedAtom > chainTail > x3::eoi;

} // namespace

Vertex addHydrogen(lib::Graph::GraphType &g, lib::Graph::PropString &pString, Vertex p) {
	Vertex v = add_vertex(g);
	pString.addVertex(v, "H");
	Edge e = add_edge(v, p, g).first;
	pString.addEdge(e, "-");
	return v;
}

bool addBond(lib::Graph::GraphType &g, lib::Graph::PropString &pString, Atom &p, Atom &v, char bond, Edge &e, std::ostream &err) {
	std::string edgeLabel;
	switch(bond) {
	case '/': // note: fall-through to make / and \ implicit
	case '\\':
		if(getConfig().graph.printSmilesParsingWarnings.get())
			IO::log() << "WARNING: up/down bonds are not supported, converted to '-' instead." << std::endl;
		[[fallthrough]];
	case 0:
		if(p.isAromatic && v.isAromatic) edgeLabel += ':';
		else edgeLabel += '-';
		break;
	case '-':
	case '=':
	case '#':
	case ':':
		edgeLabel += bond;
		break;
	case '.':
		err << "Error in SMILES conversion: the virtual bond type '.' is not (and probably will never be) supported. The SMILES string _must_ encode a single molecule." << std::endl;
		return false;
	case '$':
		err << "Error in SMILES conversion: bond type '$' is not supported." << std::endl;
		return false;
	default:
		IO::log() << "Internal error, unknown bond '" << bond << "' (" << (int) bond << ")" << std::endl;
		IO::log() << "p = '" << pString[p.vertex] << "'(" << p.vertex << ")" << std::endl;
		IO::log() << "v = '" << pString[v.vertex] << "'(" << v.vertex << ")" << std::endl;
		MOD_ABORT;
	}
	const auto ePair = add_edge(v.vertex, p.vertex, g);
	assert(ePair.second);
	e = ePair.first;
	assert(!edgeLabel.empty());
	pString.addEdge(e, edgeLabel);
	return true;
}

struct Converter {

	Converter(lib::Graph::GraphType &g, lib::Graph::PropString &pString, std::ostream &err)
	: g(g), pString(pString), err(err) { }

	bool operator()(SmilesChain &c) {
		bool res = (*this)(c.branchedAtom);
		if(!res) return false;
		res = (*this)(c.tail, c.branchedAtom.atom);
		return res;
	}

	bool operator()(ChainTail &ct, Atom &parent) {
		auto *parentPtr = &parent;
		for(BondBranchedAtomPair &bba : ct.elements) {
			bool res = (*this)(bba, *parentPtr);
			if(!res) return false;
			parentPtr = &bba.atom.get().atom;
		}
		return true;
	}

	bool operator()(BondBranchedAtomPair &bba, Atom &parent) {
		bool res = (*this)(bba.atom.get());
		if(!res) return false;
		res = addBond(g, pString, parent, bba.atom.get().atom, bba.bond, bba.edge, err);
		return res;
	}

	bool operator()(BranchedAtom &bAtom) {
		bool res = (*this)(bAtom.atom);
		if(!res) return false;
		// process ring bonds
		std::vector<char> closedRings;
		for(auto &rb : bAtom.ringBonds) {
			auto iter = openRings.find(rb.ringId);
			if(iter == end(openRings)) {
				openRings.insert(std::make_pair(rb.ringId, std::make_pair(&bAtom.atom, &rb)));
			} else {
				closedRings.push_back(rb.ringId);
				unsigned int &bondOpen = iter->second.second->bond;
				unsigned int &bondClose = rb.bond;
				if(bondOpen != 0 && bondClose != 0 && bondOpen != bondClose) {
					IO::log() << "Error in SMILES conversion: ring closure " << rb.ringId << " can not be both '" << bondOpen << "' and '" << bondClose << "'";
					MOD_ABORT;
				}
				if(bondOpen == 0) bondOpen = bondClose;
				if(bondClose == 0) bondClose = bondOpen;
				char bond = bondOpen;
				bool res = addBond(g, pString, *iter->second.first, bAtom.atom, bond, iter->second.second->edge, err);
				rb.edge = iter->second.second->edge;
				if(!res) return false;
			}
		}
		// then delete those we closed
		for(char c : closedRings) openRings.erase(c);

		for(auto &b : bAtom.branches) {
			bool res = (*this)(b, bAtom.atom);
			if(!res) return false;
		}
		return true;
	}

	bool operator()(Atom &a) {
		std::string label;
		if(a.isotope > 0)
			label += boost::lexical_cast<std::string>(a.isotope);
		if(a.symbol == "*") {
			a.isAromatic = false;
			a.atomId = AtomIds::Invalid;
			a.vertex = add_vertex(g);
			label = "*"; // overwrite on purpose
		} else {
			a.isAromatic = false;
			a.atomId = atomIdFromSymbol(a.symbol);
			if(a.atomId == AtomIds::Invalid) {
				using namespace AtomIds;
				static const std::map<std::string, AtomId> aromaticSymbols{
					{"b", Boron},
					{"c", Carbon},
					{"n", Nitrogen},
					{"o", Oxygen},
					{"p", Phosphorus},
					{"s", Sulfur},
					{"se", Selenium},
					{"as", Arsenic}};
				auto iter = aromaticSymbols.find(a.symbol);
				if(iter != end(aromaticSymbols)) {
					a.isAromatic = true;
					a.atomId = iter->second;
				} else {
					IO::log() << "Internal error, could not find '" << a.symbol << "' as atom symbol (implicit = "
							<< std::boolalpha << a.isImplicit << ")." << std::endl;
					MOD_ABORT;
				}
			}
			a.vertex = add_vertex(g);
			label += symbolFromAtomId(a.atomId);
			if(!a.isImplicit) {
				(*this)(a.chiral);
				{ // charge
					unsigned char absCharge = std::abs(a.charge);
					if(absCharge > 1) {
						if(absCharge > 9) {
							err << "Error in SMILES conversion, charge 'abs(" << a.charge << ")' is too large. The lazy developer currently assumes 9 is maximum." << std::endl;
							return false;
						}
						label += ('0' + absCharge);
					}
					if(a.charge < 0) label += '-';
					else if(a.charge > 0) label += '+';
				}
				if(a.radical) label += '.';
				if(a.class_ != -1) {
					if(getConfig().graph.appendSmilesClass.get()) {
						label += ":";
						label += boost::lexical_cast<std::string>(a.class_);
					}
					classToVertexId.emplace(a.class_, a.vertex);
				}
			}
		} // end if(wildcard)
		pString.addVertex(a.vertex, label);
		return true;
	}

	void operator()(Chiral &ch) {
		if(!ch.specifier.empty())
			hasStereo = true;
	}

private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
public:
	std::map<char, std::pair<Atom*, RingBond*> > openRings;
	std::multimap<std::size_t, Vertex> classToVertexId;
	bool hasStereo = false;
private:
	std::ostream &err;
};

struct ExplicitHydrogenAdder {

	ExplicitHydrogenAdder(lib::Graph::GraphType &g, lib::Graph::PropString &pString) : g(g), pString(pString) { }

	void operator()(SmilesChain &c) {
		(*this)(c.branchedAtom);
		(*this)(c.tail);
	}

	void operator()(ChainTail &ct) {
		for(auto &&bba : ct.elements)
			(*this)(bba.atom.get());
	}

	void operator()(BranchedAtom &bAtom) {
		(*this)(bAtom.atom);
		for(auto &b : bAtom.branches) (*this)(b);
	}

	void operator()(Atom &a) {
		if(!a.isImplicit) {
			for(unsigned int i = 0; i < a.hCount; i++) {
				const auto v = addHydrogen(g, pString, a.vertex);
				a.hydrogens.push_back(v);
			}
		}
	}
private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
};

struct ImplicitHydrogenAdder {

	ImplicitHydrogenAdder(lib::Graph::GraphType &g, lib::Graph::PropString &pString) : g(g), pString(pString) { }

	void operator()(SmilesChain &c) {
		(*this)(c.branchedAtom);
		(*this)(c.tail);
	}

	void operator()(ChainTail &ct) {
		for(auto &&bba : ct.elements)
			(*this)(bba.atom.get());
	}

	void operator()(BranchedAtom &bAtom) {
		(*this)(bAtom.atom);
		for(auto &b : bAtom.branches) (*this)(b);
	}

	void operator()(Atom &a) {
		// only implicit atoms, and only non-wildcard atoms
		if(a.isImplicit && a.atomId != AtomIds::Invalid) addImplicitHydrogens(g, pString, a.vertex, a.atomId, &addHydrogen);
	}
private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
};

struct StereoConverter {

	StereoConverter(const lib::Graph::GraphType &g, const lib::Graph::PropMolecule &pMol, std::ostream &err)
	: g(g), pMol(pMol), inf(g, pMol, false), err(err) { }

	void operator()(const SmilesChain &c) {
		const Atom *next = nullptr;
		if(!c.tail.elements.empty())
			next = &c.tail.elements.front().atom.get().atom;
		(*this)(c.branchedAtom, nullptr, next);
		(*this)(c.tail, c.branchedAtom.atom);
	}

	void operator()(const ChainTail &ct, const Atom &prev) {
		const Atom *prevPtr = &prev;
		for(int i = 0; i != ct.elements.size(); ++i) {
			const Atom *nextPtr = i + 1 == ct.elements.size() ? nullptr
					: &ct.elements[i + 1].atom.get().atom;
			const auto &bba = ct.elements[i];
			(*this)(ct.elements[i].atom.get(), prevPtr, nextPtr);
			prevPtr = &bba.atom.get().atom;
		}
	}

	void operator()(const BranchedAtom &bAtom, const Atom *prev, const Atom *next) {
		if(!bAtom.atom.isImplicit) {
			const auto &ch = bAtom.atom.chiral;
			if(!ch.specifier.empty()) {
				if(getConfig().graph.ignoreStereoInSmiles.get()) {
					if(getConfig().graph.printSmilesParsingWarnings.get())
						IO::log() << "WARNING: ignoring stereochemical information (" << ch << ") in SMILES, requested by user." << std::endl;
				} else {
					if(ch.specifier == "@" || ch.specifier == "@@") {
						assignTetrahedral(ch.specifier, bAtom, prev, next);
					} else { // not @ or @@
						if(getConfig().graph.printSmilesParsingWarnings.get())
							IO::log() << "WARNING: stereochemical information (" << ch << ") in SMILES string ignored." << std::endl;
					}
				}
			}
		}
		for(auto &b : bAtom.branches)
			(*this)(b, bAtom.atom);
	}
private:

	void assignTetrahedral(const std::string &winding, const BranchedAtom &bAtom, const Atom *prev, const Atom *next) {
		const auto v = bAtom.atom.vertex;
		const std::vector<Edge> oes(out_edges(v, g).first, out_edges(v, g).second);
		if(oes.size() != 4) {
			if(getConfig().graph.printSmilesParsingWarnings.get())
				IO::log() << "WARNING: Ignoring stereo information in SMILES. Can not add tetrahedral geometry to vertex ("
				<< bAtom.atom << ") with degree " << oes.size() << ". Must be 4." << std::endl;
			return;
		}
		std::vector<bool> used(oes.size(), false);
		std::vector<Edge> edgesToAdd;
		const auto addNeighbour = [&](const auto &v) {
			const auto eIter = std::find_if(oes.begin(), oes.end(), [v, this](const auto &e) {
				return target(e, g) == v;
			});
			assert(eIter != oes.end());
			assert(!used[eIter - oes.begin()]);
			edgesToAdd.push_back(*eIter);
			used[eIter - oes.begin()] = true;
		};
		// first the previous atom
		if(prev) addNeighbour(prev->vertex);
		// ring closures
		for(const auto &rb : bAtom.ringBonds) {
			const auto vSrc = source(rb.edge, g);
			const auto vTar = target(rb.edge, g);
			assert(vSrc != vTar);
			addNeighbour(vSrc == v ? vTar : vSrc);
		}
		// hydrogens from atom property
		for(const auto &vH : bAtom.atom.hydrogens)
			addNeighbour(vH);
		// now add all branches
		for(const ChainTail &ct : bAtom.branches) {
			const auto vBranch = ct.elements.front().atom.get().atom.vertex;
			addNeighbour(vBranch);
		}
		// the tail
		if(next) addNeighbour(next->vertex);
		// And finally try to add them to the inferrence.
		// But for now, only if we are sure to add all edges.
		assert(edgesToAdd.size() == oes.size());
		hasAssigned = true;
		inf.assignGeometry(v, lib::Stereo::getGeometryGraph().tetrahedral, err);
		inf.initEmbedding(v);
		inf.fixSimpleGeometry(v);
		if(winding == "@@" && !edgesToAdd.empty()) {
			std::reverse(edgesToAdd.begin() + 1, edgesToAdd.end());
		}
		for(const auto &e : edgesToAdd) inf.addEdge(v, e);
	}
public:
	const lib::Graph::GraphType &g;
	const lib::Graph::PropMolecule &pMol;
	lib::Stereo::Inference<lib::Graph::GraphType, lib::Graph::PropMolecule> inf;
	std::ostream &err;
	bool hasAssigned = false;
};

lib::IO::Graph::Read::Data parseSmiles(const std::string &smiles, std::ostream &err) {
	using IteratorType = std::string::const_iterator;
	IteratorType iterStart = begin(smiles), iterEnd = end(smiles);
	SmilesChain ast;
	bool res = lib::IO::parse(iterStart, iterEnd, parser::smiles, ast, err);
	if(!res) return lib::IO::Graph::Read::Data();
	auto gPtr = std::make_unique<lib::Graph::GraphType>();
	auto pStringPtr = std::make_unique<lib::Graph::PropString>(*gPtr);
	std::stringstream astStr;
	astStr << ast;
	if(smiles != astStr.str()) {
		IO::log() << "Converting: >>>" << smiles << "<<< " << smiles.size() << std::endl;
		IO::log() << "Ast:        >>>" << astStr.str() << "<<< " << astStr.str().size() << std::endl;
		for(auto c : smiles) {
			if(std::isprint(c)) IO::log() << c << ' ';
			else IO::log() << int(c) << ' ';
		}
		IO::log() << std::endl;
		for(auto c : astStr.str()) {
			if(std::isprint(c)) IO::log() << c << ' ';
			else IO::log() << int(c) << ' ';
		}
		IO::log() << std::endl;
		MOD_ABORT;
	}
	Converter conv(*gPtr, *pStringPtr, err);
	res = conv(ast);
	if(!res) return lib::IO::Graph::Read::Data();
	if(conv.openRings.size() > 0) {
		err << "Error in SMILES conversion: unclosed rings (";
		auto iter = begin(conv.openRings);
		err << (int) iter->first;
		for(iter++; iter != end(conv.openRings); iter++) err << ", " << (int) iter->first;
		err << ")" << std::endl;
		return lib::IO::Graph::Read::Data();
	}
	(ExplicitHydrogenAdder(*gPtr, *pStringPtr))(ast);
	(ImplicitHydrogenAdder(*gPtr, *pStringPtr)(ast));
	lib::IO::Graph::Read::Data data;
	if(conv.hasStereo) {
		lib::Graph::PropMolecule pMol(*gPtr, *pStringPtr);
		StereoConverter stereoConv(*gPtr, pMol, err);
		stereoConv(ast);
		if(stereoConv.hasAssigned) {
			std::stringstream finalizeErr;
			auto deductionResult = stereoConv.inf.finalize(finalizeErr, [&](auto v) {
				return get(boost::vertex_index_t(), *gPtr, v);
			});
			switch(deductionResult) {
			case lib::Stereo::DeductionResult::Success: break;
			case lib::Stereo::DeductionResult::Warning:
				if(!getConfig().stereo.silenceDeductionWarnings.get())
					IO::log() << finalizeErr.str();
				break;
			case lib::Stereo::DeductionResult::Error:
				err << finalizeErr.str();
				return lib::IO::Graph::Read::Data();
			}
			data.pStereo = std::make_unique<lib::Graph::PropStereo>(*gPtr, std::move(stereoConv.inf));
		}
	}
	data.g = std::move(gPtr);
	data.pString = std::move(pStringPtr);
	bool isValid = std::all_of(conv.classToVertexId.begin(), conv.classToVertexId.end(), [&conv](auto &vp) {
		return conv.classToVertexId.count(vp.first) == 1;
	});
	if(isValid) {
		for(auto &&vp : conv.classToVertexId) {
			data.externalToInternalIds[vp.first] = get(boost::vertex_index_t(), *gPtr, vp.second);
		}
	}
	return data;
}

} // namespace
} // namespace Smiles

lib::IO::Graph::Read::Data readSmiles(const std::string &smiles, std::ostream &err) {
	return Smiles::parseSmiles(smiles, err);
}

} // namespace Chem
} // namespace lib
} // namespace mod

BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::Chiral,
		(std::string, specifier)
		(unsigned int, k)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::Atom,
		(unsigned int, isotope)
		(std::string, symbol)
		(mod::lib::Chem::Smiles::Chiral, chiral)
		(unsigned int, hCount)
		(mod::lib::Chem::Smiles::Charge, charge)
		(bool, radical)
		(int, class_)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::BondBranchedAtomPair,
		(unsigned int, bond)
		(x3::forward_ast<mod::lib::Chem::Smiles::BranchedAtom>, atom)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::ChainTail,
		(std::vector<mod::lib::Chem::Smiles::BondBranchedAtomPair>, elements)
		(int, dummy)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::RingBond,
		(unsigned int, bond)
		(unsigned int, ringId)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::BranchedAtom,
		(mod::lib::Chem::Smiles::Atom, atom)
		(std::vector<mod::lib::Chem::Smiles::RingBond>, ringBonds)
		(std::vector<mod::lib::Chem::Smiles::ChainTail>, branches)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::SmilesChain,
		(mod::lib::Chem::Smiles::BranchedAtom, branchedAtom)
		(mod::lib::Chem::Smiles::ChainTail, tail)
		)