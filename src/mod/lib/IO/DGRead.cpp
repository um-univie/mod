#include "DG.h"

#include <mod/graph/Graph.h>
#include <mod/rule/Rule.h>
#include <mod/lib/DG/Dump.h>
#include <mod/lib/DG/NonHyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/ParsingUtil.h>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/and_predicate.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

#include <string>
#include <unordered_map>

namespace mod {
namespace lib {
namespace IO {
namespace DG {
namespace Read {

struct Derivation {
	using List = std::vector<std::pair<unsigned int, std::string> >;
	List left;
	bool reverse;
	List right;
public:

	friend bool operator==(const Derivation &l, const Derivation &r) {
		return std::tie(l.left, l.reverse, l.right) == std::tie(r.left, r.reverse, r.right);
	}
};

namespace {

struct NonHyperAbstract : public lib::DG::NonHyper {
	NonHyperAbstract(const std::vector<Derivation> &derivations)
	: NonHyper({},
	{

		LabelType::String, LabelRelation::Isomorphism, false, LabelRelation::Isomorphism
	}), derivationsForConstruction(&derivations) {
		calculate(true);
	}
private:

	virtual std::string getType() const override {
		return "DGAbstract";
	}

	virtual void calculateImpl(bool printInfo) override {
		const auto &derivations = *derivationsForConstruction;
		std::unordered_map<std::string, std::shared_ptr<graph::Graph> > strToGraph;
		for(const auto &der : derivations) {
			const auto handleSide = [this, &strToGraph] (const Derivation::List & side) {
				for(const auto &e : side) {
					const auto iter = strToGraph.find(e.second);
					if(iter == end(strToGraph)) {
						auto gBoost = std::make_unique<lib::Graph::GraphType>();
						auto pString = std::make_unique<lib::Graph::PropString>(*gBoost);
						auto gLib = std::make_unique<lib::Graph::Single>(std::move(gBoost), std::move(pString), nullptr);
						auto g = graph::Graph::makeGraph(std::move(gLib));
						addProduct(g); // this renames it
						g->setName(e.second);
						strToGraph[e.second] = g;
					}
				}
			};
			handleSide(der.left);
			handleSide(der.right);
		}
		for(const auto &der : derivations) {
			using Side = std::unordered_map<std::shared_ptr<graph::Graph>, unsigned int>;
			const auto makeSide = [&strToGraph] (const Derivation::List & side) {
				Side result;
				for(const auto &e : side) {
					const auto g = strToGraph[e.second];
					assert(g);
					auto iter = result.find(g);
					if(iter == end(result)) iter = result.insert(std::make_pair(g, 0)).first;
					iter->second += e.first;
				}
				return result;
			};
			const Side left = makeSide(der.left);
			const Side right = makeSide(der.right);
			std::vector<const lib::Graph::Single*> leftGraphs, rightGraphs;
			for(const auto &e : left) {
				for(unsigned int i = 0; i < e.second; i++)
					leftGraphs.push_back(&e.first->getGraph());
			}
			for(const auto &e : right) {
				for(unsigned int i = 0; i < e.second; i++)
					rightGraphs.push_back(&e.first->getGraph());
			}
			lib::DG::GraphMultiset gmsLeft(std::move(leftGraphs)), gmsRight(std::move(rightGraphs));
			suggestDerivation(gmsLeft, gmsRight, nullptr);
			if(der.reverse) {
				suggestDerivation(gmsRight, gmsLeft, nullptr);
			}
		}
		derivationsForConstruction = nullptr;
	}

	virtual void listImpl(std::ostream &s) const override { }
private:
	std::vector<std::shared_ptr<rule::Rule> > rules;
	const std::vector<Derivation> *derivationsForConstruction;
};

} // namespace 
} // namespace Read
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod

BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DG::Read::Derivation,
		(mod::lib::IO::DG::Read::Derivation::List, left)
		(bool, reverse)
		(mod::lib::IO::DG::Read::Derivation::List, right)
		);

namespace mod {
namespace lib {
namespace IO {
namespace DG {
namespace Read {
namespace {
namespace parser {

const auto identifier = x3::lexeme[*(x3::char_ - x3::space)];
const auto coef = x3::lexeme[x3::uint_ >> &x3::space] | (x3::eps >> x3::attr(1u));
const auto element = x3::rule<struct element, std::pair<unsigned int, std::string> >("element")
/*              */ = coef >> identifier;
const auto side = element % '+';
const auto arrow = ("->" >> x3::attr(false)) | ("<=>" >> x3::attr(true));
const auto derivation = x3::rule<struct arrow, Derivation>("derivation")
/*                 */ = side >> arrow >> side;
const auto derivations = +derivation;

} // namespace parser
} // namespace

lib::DG::NonHyper *dump(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, const std::string &file, std::ostream &err) {
	std::ifstream fileInStream(file.c_str());
	if(!fileInStream.is_open()) {
		err << "DG file not found, '" << file << "'" << std::endl;
		return nullptr;
	}
	return lib::DG::Dump::load(graphs, rules, fileInStream, err);
}

lib::DG::NonHyper *abstract(const std::string &s, std::ostream &err) {
	auto iterStart = s.begin(), iterEnd = s.end();
	std::vector<Derivation> derivations;
	bool res = lib::IO::parse(iterStart, iterEnd, parser::derivations, derivations, err, x3::space);
	if(!res) return nullptr;
	return new NonHyperAbstract(derivations);
}

} // namespace Read
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod
