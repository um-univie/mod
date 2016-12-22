// see http://www.opensmiles.org/opensmiles.html

#include "Smiles.h"

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/ParserCommon.h>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_uint.hpp>

#include <map>

using namespace mod::lib::IO::Parser;

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
public:
	int charge;
};

struct Chiral {
	std::string specifier;
	unsigned int k;

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
				if(a.hCount > 1) s << (int) a.hCount;
			}
			if(a.charge != 0) {
				s << (a.charge < 0 ? '-' : '+');
				if(std::abs(a.charge) > 1) s << (int) std::abs(a.charge);
			}
			if(a.radical) s << '.';
			if(a.class_ != -1) s << ":" << a.class_;
			s << "]";
		}
		return s;
	}
};

struct Chain;
std::ostream &operator<<(std::ostream &s, const Chain &c);

struct Branch {
	unsigned int bond;
	boost::recursive_wrapper<Chain> tail;
public:
	bool isImplicit;
	Edge edge;

	friend std::ostream &operator<<(std::ostream &s, const Branch &b) {
		if(b.bond != 0) s << (char) b.bond;
		return s << b.tail.get();
	}
};

struct RingBond {
	unsigned int bond;
	unsigned int ringId;
public:
	Edge edge;

	friend std::ostream &operator<<(std::ostream &s, const RingBond &rb) {
		if(rb.bond != 0) s << (char) rb.bond;
		if(rb.ringId > 9) s << "%";
		return s << rb.ringId;
	}
};

struct BranchedAtom {
	Atom atom;
	std::vector<RingBond> ringBonds;
	std::vector<Branch> branches;

	friend std::ostream &operator<<(std::ostream &s, const BranchedAtom &ba) {
		s << ba.atom;
		for(const auto &rb : ba.ringBonds) s << rb;
		for(const auto &b : ba.branches) s << '(' << b << ')';
		return s;
	}
};

struct Chain {
	BranchedAtom branchedAtom;
	boost::optional<Branch> tail;

	friend std::ostream &operator<<(std::ostream &s, const Chain &c) {
		s << c.branchedAtom;
		if(c.tail) s << *c.tail;
		return s;
	}
};

} // namespace Smiles
} // namespace Chem
} // namespace lib
} // namespace mod
namespace boost {

std::ostream &operator<<(std::ostream &s, const boost::recursive_wrapper<mod::lib::Chem::Smiles::Chain> &chain) {
	return s << chain.get();
}

} // namespace boost
namespace mod {
namespace lib {
namespace Chem {
namespace Smiles {
namespace {

template<typename Iter>
struct Parser : public qi::grammar<Iter, Chain()> {

	Parser(std::ostream &s) : Parser::base_type(start), errorHandler(s) {
		this->name("smiles");
		start %= chain.alias();
		start.name("smiles");
		chain %= branchedAtom > chainTail;
		chain.name("chain");
		chainTail %= (bond >> chain) | qi::eps;
		chainTail.name("chainTail");
		dot %= qi::char_('.');
		dot.name("dot");
		branchedAtom %= atom > ringBonds > branches;
		branchedAtom.name("branchedAtom");
		bond %= bondSymbol | dot | qi::attr(0);
		bond.name("bond");
		ringBonds %= *ringBond;
		ringBonds.name("ringBonds");
		ringBond %= (bondSymbol | qi::attr(0)) >> ringId;
		ringBond.name("ringBond");
		ringId %= singleDigit | (qi::lit('%') > doubleDigit);
		ringId.name("ringId");
		branches %= (branch > branches) | qi::eps;
		branches.name("branches");
		branch %= qi::lit('(') > bond >> chain > ')';
		branch.name("branch");
		atom %= bracketAtom | shorthandAtom;
		atom.name("atom");
		bracketAtom %= qi::lit('[') > isotope > symbol > chiral > hcount > charge > radical > class_ > ']';
		bracketAtom.name("bracketAtom");
		shorthandAtom %= shorthandSymbol;
		shorthandAtom.name("aliphaticOrganicAtom|aromaticOrganicAtom|implicitWildcardAtom");
		symbol %= elementSymbol | aromaticSymbol | qi::string("*");
		symbol.name("symbol");
		isotope %= qi::uint_ | qi::attr(0);
		isotope.name("isotope");
		chiral = (chiralSymbol >> qi::attr(0))
				| ((qi::string("@TB") | qi::string("@OH")) > doubleDigit)
				| (qi::attr("") >> qi::attr(0));
		chiral.name("chiral");
		hcount %= (qi::lit('H') > (singleDigit | qi::attr(1))) // singleDigit may NOT parse unary + or -
				| qi::attr(0);
		hcount.name("hcount");
		chargeNum %= doubleDigit | qi::attr(1);
		chargeNum.name("chargeNum");
		charge %= (qi::char_("+-") | qi::attr(0)) >> chargeNum;
		charge.name("charge");
		radical %= (qi::lit('.') >> qi::attr(true)) | qi::attr(false);
		radical.name("radical");
		class_ %= (qi::lit(':') > qi::uint_)
				| qi::attr(-1);
		class_.name("class");

		for(auto atomId : getSmilesOrganicSubset())
			shorthandSymbol.add(symbolFromAtomId(atomId), symbolFromAtomId(atomId));
		for(std::string str :{"b", "c", "n", "o", "s", "p"})
			shorthandSymbol.add(str, str);
		shorthandSymbol.add("*", "*");
		shorthandSymbol.name("aliphaticOrganic|aromaticOrganic|wildcardSymbol");
		for(char c :{'-', '=', '#', '$', ':', '/', '\\'}) bondSymbol.add(std::string(1, c), c);
		bondSymbol.name("bondSymbol");
		for(unsigned char i = 1; i <= AtomIds::Max; i++)
			elementSymbol.add(symbolFromAtomId(AtomId(i)), symbolFromAtomId(AtomId(i)));
		elementSymbol.name("elementSymbol");
		for(std::string str :{"b", "c", "n", "o", "p", "s", "se", "as"}) aromaticSymbol.add(str, str);
		aromaticSymbol.name("aromaticSymbol");
		for(std::string str :{"@", "@@", "@TH1", "@TH2", "@AL1", "@AL2", "@SP1", "@SP2", "@SP3"}) chiralSymbol.add(str, str);
		for(unsigned int i = 1; i < 20; i++) chiralSymbol.add("@TB" + boost::lexical_cast<std::string>(i), "@TB" + boost::lexical_cast<std::string>(i));
		for(unsigned int i = 1; i < 30; i++) chiralSymbol.add("@OH" + boost::lexical_cast<std::string>(i), "@OH" + boost::lexical_cast<std::string>(i));
		chiralSymbol.name("chiralSymbol");

		qi::on_error<qi::fail>(start, errorHandler(qi::_1, qi::_2, qi::_3, qi::_4));
	}
private:
	qi::rule<Iter, Chain() > start, chain;
	qi::rule<Iter, boost::optional<Branch>() > chainTail;
	qi::rule<Iter, BranchedAtom() > branchedAtom;
	qi::rule<Iter, unsigned int() > bond, dot;
	qi::rule<Iter, std::vector<Branch>() > branches;
	qi::rule<Iter, Branch() > branch;
	qi::rule<Iter, std::vector<RingBond>() > ringBonds;
	qi::rule<Iter, RingBond() > ringBond;
	qi::rule<Iter, unsigned int() > ringId;
	qi::rule<Iter, Atom() > atom, bracketAtom, shorthandAtom;
	qi::rule<Iter, std::string() > symbol;
	qi::rule<Iter, unsigned int() > isotope;
	qi::rule<Iter, Chiral() > chiral;
	qi::rule<Iter, unsigned int() > hcount;
	qi::rule<Iter, int() > chargeNum;
	qi::rule<Iter, std::pair<char, int>() > charge;
	qi::rule<Iter, bool() > radical;
	qi::rule<Iter, unsigned int() > class_;
	qi::symbols<char, std::string> shorthandSymbol, elementSymbol, aromaticSymbol;
	qi::symbols<char, unsigned int> bondSymbol;
	qi::symbols<char, std::string> chiralSymbol;
	qi::uint_parser<int, 10, 1, 1> singleDigit;
	qi::uint_parser<int, 10, 1, 2> doubleDigit;
	phx::function<ErrorHandler<Iter> > errorHandler;
};

void addHydrogen(lib::Graph::GraphType &g, lib::Graph::PropString &pString, Vertex p) {
	Vertex v = add_vertex(g);
	pString.addVertex(v, "H");
	Edge e = add_edge(v, p, g).first;
	pString.addEdge(e, "-");
}

bool addBond(lib::Graph::GraphType &g, lib::Graph::PropString &pString, Atom &p, Atom &v, char bond, Edge &e, std::ostream &err) {
	std::string edgeLabel;
	switch(bond) {
	case '/': // note: fall-through to make / and \ implicit
	case '\\':
		if(getConfig().graph.printSmilesParsingWarnings.get())
			IO::log() << "WARNING: up/down bonds are not supported, converted to '-' instead." << std::endl;
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
	e = add_edge(v.vertex, p.vertex, g).first;
	assert(!edgeLabel.empty());
	pString.addEdge(e, edgeLabel);
	return true;
}

struct Converter {

	Converter(lib::Graph::GraphType &g, lib::Graph::PropString &pString, std::ostream &err)
	: g(g), pString(pString), parent(nullptr), status(true), err(err) { }

	void operator()(Chain &c) {
		(*this)(c.branchedAtom);
		if(!status) return;
		if(c.tail) (*this)(*c.tail);
	}

	void operator()(BranchedAtom &bAtom) {
		(*this)(bAtom.atom);
		if(!status) return;
		parent = &bAtom.atom;
		// process ring bonds
		std::vector<char> closedRings;
		for(auto &rb : bAtom.ringBonds) {
			auto iter = openRings.find(rb.ringId);
			if(iter == end(openRings)) {
				openRings.insert(std::make_pair(rb.ringId, std::make_pair(parent, &rb)));
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
				status = addBond(g, pString, *iter->second.first, *parent, bond, iter->second.second->edge, err);
			}
		}
		// then delete those we closed
		for(char c : closedRings) openRings.erase(c);

		for(auto &b : bAtom.branches) {
			(*this)(b);
			if(!status) return;
			parent = &bAtom.atom;
		}
	}

	void operator()(Branch &b) {
		parentBond = b.bond;
		parentEdge = &b.edge;
		(*this)(b.tail.get());
	}

	void operator()(Chiral &ch) {
		if(!ch.specifier.empty()) {
			if(getConfig().graph.printSmilesParsingWarnings.get())
				IO::log() << "WARNING: stereochemical information (" << ch << ") in SMILES string ignored." << std::endl;
		}
	}

	void operator()(Atom &a) {
		if(a.isotope > 0) {
			if(getConfig().graph.printSmilesParsingWarnings.get())
				IO::log() << "WARNING: isotope information in SMILES string ignored." << std::endl;
		}
		std::string label;
		if(a.symbol == "*") {
			a.isAromatic = false;
			a.atomId = AtomIds::Invalid;
			a.vertex = add_vertex(g);
			label = "*";
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
					{"as", Arsenic}
				};
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
			label = symbolFromAtomId(a.atomId);
			if(!a.isImplicit) {
				(*this)(a.chiral);
				if(!status) return;
				{ // charge
					unsigned char absCharge = std::abs(a.charge);
					if(absCharge > 1) {
						if(absCharge > 9) {
							err << "Error in SMILES conversion, charge 'abs(" << a.charge << ")' is too large. The lazy developer currently assumes 9 is maximum." << std::endl;
							status = false;
							return;
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
		if(parent) status = addBond(g, pString, *parent, a, parentBond, *parentEdge, err);
	}
private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
	Atom *parent;
	char parentBond;
	Edge *parentEdge;
public:
	bool status;
	std::map<char, std::pair<Atom*, RingBond*> > openRings;
	std::multimap<std::size_t, Vertex> classToVertexId;
private:
	std::ostream &err;
};

struct ExplicitHydrogenAdder {

	ExplicitHydrogenAdder(lib::Graph::GraphType &g, lib::Graph::PropString &pString) : g(g), pString(pString) { }

	void operator()(Chain &c) {
		(*this)(c.branchedAtom);
		if(c.tail) (*this)(*c.tail);
	}

	void operator()(BranchedAtom &bAtom) {
		(*this)(bAtom.atom);
		for(auto &b : bAtom.branches) (*this)(b);
	}

	void operator()(Branch &b) {
		(*this)(b.tail.get());
	}

	void operator()(Atom &a) {
		if(!a.isImplicit)
			for(unsigned int i = 0; i < a.hCount; i++)
				addHydrogen(g, pString, a.vertex);
	}
private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
};

struct ImplicitHydrogenAdder {

	ImplicitHydrogenAdder(lib::Graph::GraphType &g, lib::Graph::PropString &pString) : g(g), pString(pString) { }

	void operator()(Chain &c) {
		(*this)(c.branchedAtom);
		if(c.tail) (*this)(*c.tail);
	}

	void operator()(BranchedAtom &bAtom) {
		(*this)(bAtom.atom);
		for(auto &b : bAtom.branches) (*this)(b);
	}

	void operator()(Branch &b) {
		(*this)(b.tail.get());
	}

	void operator()(Atom &a) {
		// only implicit atoms, and only non-wildcard atoms
		if(a.isImplicit && a.atomId != AtomIds::Invalid) addImplicitHydrogens(g, pString, a.vertex, a.atomId, &addHydrogen);
	}
private:
	lib::Graph::GraphType &g;
	lib::Graph::PropString &pString;
};

lib::IO::Graph::Read::Data parseSmiles(const std::string &smiles, std::ostream &err) {
	using IteratorType = std::string::const_iterator;
	IteratorType iterStart = begin(smiles), iterEnd = end(smiles);
	Parser<IteratorType> parser(err);
	Chain ast;
	bool res = IO::Parser::parse(err, iterStart, iterEnd, parser, ast);
	if(!res) return lib::IO::Graph::Read::Data();
	auto gPtr = std::make_unique<lib::Graph::GraphType>();
	auto pStringPtr = std::make_unique<lib::Graph::PropString>(*gPtr);
	std::stringstream astStr;
	astStr << ast;
	if(smiles != astStr.str()) {
		IO::log() << "Converting: " << smiles << std::endl;
		IO::log() << "Ast:        " << ast << std::endl;
		MOD_ABORT;
	}
	Converter conv(*gPtr, *pStringPtr, err);
	conv(ast);
	if(!conv.status) return lib::IO::Graph::Read::Data();
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
	auto data = lib::IO::Graph::Read::Data(std::move(gPtr), std::move(pStringPtr));
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
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::Branch,
		(unsigned int, bond)
		(boost::recursive_wrapper<mod::lib::Chem::Smiles::Chain>, tail)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::RingBond,
		(unsigned int, bond)
		(unsigned int, ringId)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::Chain,
		(mod::lib::Chem::Smiles::BranchedAtom, branchedAtom)
		(boost::optional<mod::lib::Chem::Smiles::Branch>, tail)
		)
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Chem::Smiles::BranchedAtom,
		(mod::lib::Chem::Smiles::Atom, atom)
		(std::vector<mod::lib::Chem::Smiles::RingBond>, ringBonds)
		(std::vector<mod::lib::Chem::Smiles::Branch>, branches)
		)
