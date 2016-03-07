#include "DG.h"

#include <mod/Graph.h>
#include <mod/Rule.h>
#include <mod/lib/Chem/SBML.h>
#include <mod/lib/DG/Dump.h>
#include <mod/lib/DG/NonHyper.h>
#include <mod/lib/Graph/Merge.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/ParserCommon.h>
#include <mod/lib/Rule/Shallow.h>

#include <jla_boost/Memory.hpp>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_uint.hpp>

#include <string>
#include <unordered_map>

using namespace mod::lib::IO::Parser;

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
};
namespace {

struct NonHyperAbstract : public lib::DG::NonHyper {

	NonHyperAbstract(const std::vector<Derivation> &derivations)
	: NonHyper({}), derivationsForConstruction(&derivations) {
		calculate();
	}
private:

	std::string getType() const {
		return "DGAbstract";
	}

	void calculateImpl() {
		const auto &derivations = *derivationsForConstruction;
		std::unordered_map<std::string, std::shared_ptr<mod::Graph> > strToGraph;
		for(const auto &der : derivations) {
			auto handleSide = [this, &strToGraph] (const Derivation::List & side) {
				for(const auto &e : side) {
					auto iter = strToGraph.find(e.second);
					if(iter == end(strToGraph)) {
						auto gBoost = make_unique<lib::Graph::GraphType>();
						auto pString = make_unique<lib::Graph::PropStringType>(*gBoost);
						auto gLib = make_unique<lib::Graph::Single>(std::move(gBoost), std::move(pString));
						auto g = mod::Graph::makeGraph(std::move(gLib));
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
			typedef std::unordered_map<std::shared_ptr<mod::Graph>, unsigned int> Side;
			auto makeSide = [&strToGraph] (const Derivation::List & side) {
				Side result;
				for(const auto &e : side) {
					auto g = strToGraph[e.second];
					assert(g);
					auto iter = result.find(g);
					if(iter == end(result)) iter = result.insert(std::make_pair(g, 0)).first;
					iter->second += e.first;
				}
				return result;
			};
			Side left = makeSide(der.left);
			Side right = makeSide(der.right);
			auto r = mod::Rule::makeRule(make_unique<lib::Rule::Shallow>());
			rules.push_back(r);
			auto *leftMerge = new lib::Graph::Merge();
			auto *rightMerge = new lib::Graph::Merge();
			for(const auto &e : left) {
				for(unsigned int i = 0; i < e.second; i++) leftMerge->mergeWith(e.first->getGraph());
			}
			for(const auto &e : right) {
				for(unsigned int i = 0; i < e.second; i++) rightMerge->mergeWith(e.first->getGraph());
			}
			leftMerge->lock();
			rightMerge->lock();
			const lib::Graph::Base *leftLinked = leftMerge;
			if(leftMerge->getSingles().size() == 1) {
				leftLinked = *begin(leftMerge->getSingles());
				delete leftMerge;
			} else {
				leftLinked = addToMergeStore(leftMerge);
			}
			const lib::Graph::Base *rightLinked = rightMerge;
			if(rightMerge->getSingles().size() == 1) {
				rightLinked = *begin(rightMerge->getSingles());
				delete rightMerge;
			} else {
				rightLinked = addToMergeStore(rightMerge);
			}
			suggestDerivation(leftLinked, rightLinked, &r->getBase());
			if(der.reverse) {
				auto r = mod::Rule::makeRule(make_unique<lib::Rule::Shallow>());
				rules.push_back(r);
				suggestDerivation(rightLinked, leftLinked, &r->getBase());
			}
		}
		derivationsForConstruction = nullptr;
	}

	void listImpl(std::ostream &s) const { }
private:
	std::vector<std::shared_ptr<mod::Rule> > rules;
	const std::vector<Derivation> *derivationsForConstruction;
};

} // namespace 
namespace {

template<typename Parser, typename Attr>
bool parse(IteratorType &iterStart, IteratorType iterEnd, Parser &parser, Attr &attr, std::ostream &err) {
	bool res = qi::phrase_parse(iterStart, iterEnd, parser, qi::space, attr);
	if(!res || iterStart != iterEnd) {
		err << "Error while parsing abstract DG specification at:" << std::endl;
		IteratorType iterCheck = iterStart;
		unsigned int numNewLine = 0;
		for(iterCheck++; numNewLine < 2 && iterCheck != iterEnd; iterCheck++)
			if(*iterCheck == '\n') numNewLine++;
		err << (std::string(iterStart, iterCheck)) << std::endl << "(end of code snippet)" << std::endl;
		if(!res) err << "Parsing explicitly failed." << std::endl;
		else if(iterStart != iterEnd) err << "Could not parse all input." << std::endl;
		return false;
	}
	return res;
}

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

lib::DG::Matrix *sbml(const std::string &file, std::ostream &err) {
	std::ifstream fileInStream(file.c_str());
	if(!fileInStream.is_open()) {
		err << "SBML file not found, '" << file << "'" << std::endl;
		return nullptr;
	}
	fileInStream.close();
	return Chem::loadNetworkFromSBML(file);
}

lib::DG::NonHyper *dump(const std::vector<std::shared_ptr<mod::Graph> > &graphs, const std::vector<std::shared_ptr<mod::Rule> > &rules, const std::string &file, std::ostream &err) {
	std::ifstream fileInStream(file.c_str());
	if(!fileInStream.is_open()) {
		err << "DG file not found, '" << file << "'" << std::endl;
		return nullptr;
	}
	return lib::DG::Dump::load(graphs, rules, fileInStream, err);
}

lib::DG::NonHyper *abstract(std::istream &s, std::ostream &err) {
	std::ios::fmtflags oldFlags = s.flags();
	s.unsetf(std::ios::skipws);
	IteratorType iterStart = spirit::make_default_multi_pass(BaseIteratorType(s));
	IteratorType iterRealStart = iterStart;
	IteratorType iterEnd;

	std::vector<Derivation> derivations;

	QiRule < std::string()>::type grIdentifier = qi::lexeme[qi::char_("A-Za-z_") >> *qi::char_("A-Za-z0-9:_-")];
	QiRule<unsigned int()>::type grCoef = qi::uint_ | (qi::eps >> qi::attr(1));
	QiRule < std::pair<unsigned int, std::string>()>::type element = grCoef >> grIdentifier;
	QiRule < Derivation::List()>::type grSide = element % '+';
	QiRule<bool()>::type grArrow = ("->" >> qi::attr(false)) | ("<=>" >> qi::attr(true));
	QiRule < Derivation()>::type grDerivation = grSide >> grArrow >> grSide;
	QiRule < std::vector<Derivation>()>::type grammar = +grDerivation;
	bool res = parse(iterStart, iterEnd, grammar, derivations, err);
	s.flags(oldFlags);
	if(!res) return nullptr;

	IO::log() << "Data:" << std::endl;
	for(const auto &der : derivations) {
		auto printSide = [](const Derivation::List & side) {
			for(unsigned int i = 0; i < side.size(); i++) {
				if(i > 0) IO::log() << " + ";
				if(side[i].first != 1) IO::log() << side[i].first << " ";
				IO::log() << side[i].second;
			}
		};
		printSide(der.left);
		if(der.reverse) IO::log() << " <=> ";
		else IO::log() << " -> ";
		printSide(der.right);
		IO::log() << std::endl;
	}
	return new NonHyperAbstract(derivations);
}

} // namespace Read
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod
