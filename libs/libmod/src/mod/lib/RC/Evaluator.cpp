#include "Evaluator.hpp"

#include <mod/Config.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/rule/CompositionExpr.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/IO/RC.hpp>
#include <mod/lib/RC/ComposeRuleReal.hpp>
#include <mod/lib/RC/MatchMaker/Common.hpp>
#include <mod/lib/RC/MatchMaker/Parallel.hpp>
#include <mod/lib/RC/MatchMaker/Sub.hpp>
#include <mod/lib/RC/MatchMaker/Super.hpp>

#include <boost/variant/static_visitor.hpp>

namespace mod {
namespace lib {
namespace RC {
namespace {

struct EvalVisitor : public boost::static_visitor<std::unordered_set<std::shared_ptr<rule::Rule> > > {

	EvalVisitor(Evaluator &evaluator) : evaluator(evaluator) { }

	// Nullary/unary
	//----------------------------------------------------------------------

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(std::shared_ptr<rule::Rule> r) {
		return std::unordered_set<std::shared_ptr<rule::Rule> >{r};
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::Union &par) {
		std::unordered_set<std::shared_ptr<rule::Rule> > result;
		for(const auto &subExp : par.getExpressions()) {
			auto subRes = subExp.applyVisitor(*this);
			result.insert(begin(subRes), end(subRes));
		}
		return result;
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::Bind &bind) {
		std::unordered_set<std::shared_ptr<rule::Rule> > result;
		result.insert(bind.getGraph()->getGraph().getBindRule());
		return result;
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::Id &id) {
		std::unordered_set<std::shared_ptr<rule::Rule> > result;
		result.insert(id.getGraph()->getGraph().getIdRule());
		return result;
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::Unbind &unbind) {
		std::unordered_set<std::shared_ptr<rule::Rule> > result;
		result.insert(unbind.getGraph()->getGraph().getUnbindRule());
		return result;
	}

	// Binary
	//----------------------------------------------------------------------

	template<typename Composer>
	std::unordered_set<std::shared_ptr<rule::Rule> > composeTemplate(const rule::RCExp::ComposeBase &compose, Composer composer) {
		auto firstResult = compose.getFirst().applyVisitor(*this);
		auto secondResult = compose.getSecond().applyVisitor(*this);
		std::unordered_set<std::shared_ptr<rule::Rule> > result;
		for(auto rFirst : firstResult) {
			for(auto rSecond : secondResult) {
				std::vector<lib::Rules::Real*> resultVec;
				auto reporter = [&resultVec] (std::unique_ptr<lib::Rules::Real> r) {
					resultVec.push_back(r.release());
					return true;
				};
				composer(rFirst->getRule(), rSecond->getRule(), reporter);
				for(auto *r : resultVec) {
					if(compose.getDiscardNonchemical() && !r->isChemical()) {
						delete r;
						continue;
					}
					auto rWrapped = evaluator.checkIfNew(r);
					bool isNew = evaluator.addRule(rWrapped);
					if(isNew) evaluator.giveProductStatus(rWrapped);
					evaluator.suggestComposition(&rFirst->getRule(), &rSecond->getRule(), &rWrapped->getRule());
					result.insert(rWrapped);
				}
			}
		}
		return result;
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::ComposeCommon &common) {
		const auto composer = [&common, this](const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond,
				std::function<bool(std::unique_ptr<lib::Rules::Real>) > reporter) {
			RC::Common mm(common.getMaxmimum(), common.getConnected());
			lib::RC::composeRuleRealByMatchMaker(rFirst, rSecond, mm, reporter, evaluator.labelSettings);
		};
		return composeTemplate(common, composer);
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::ComposeParallel &common) {
		const auto composer = [this](const lib::Rules::Real &rFirst, const lib::Rules::Real & rSecond,
				std::function<bool(std::unique_ptr<lib::Rules::Real>) > reporter) {
			lib::RC::composeRuleRealByMatchMaker(rFirst, rSecond, lib::RC::Parallel(), reporter, evaluator.labelSettings);
		};
		return composeTemplate(common, composer);
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::ComposeSub &sub) {
		const auto composer = [&sub, this](const lib::Rules::Real &rFirst, const lib::Rules::Real & rSecond,
				std::function<bool(std::unique_ptr<lib::Rules::Real>) > reporter) {
			RC::Sub mm(sub.getAllowPartial());
			lib::RC::composeRuleRealByMatchMaker(rFirst, rSecond, mm, reporter, evaluator.labelSettings);
		};
		return composeTemplate(sub, composer);
	}

	std::unordered_set<std::shared_ptr<rule::Rule> > operator()(const rule::RCExp::ComposeSuper &super) {
		const auto composer = [&super, this](const lib::Rules::Real &rFirst, const lib::Rules::Real & rSecond,
				std::function<bool(std::unique_ptr<lib::Rules::Real>) > reporter) {
			RC::Super mm(super.getAllowPartial(), super.getEnforceConstraints());
			lib::RC::composeRuleRealByMatchMaker(rFirst, rSecond, mm, reporter, evaluator.labelSettings);
		};
		return composeTemplate(super, composer);
	}
private:
	Evaluator &evaluator;
};

} // namespace 

Evaluator::Evaluator(std::unordered_set<std::shared_ptr<rule::Rule> > database, LabelSettings labelSettings)
: labelSettings(labelSettings), database(database) {
	if(labelSettings.type == LabelType::Term) {
		for(const auto r : database) {
			const auto &term = get_term(r->getRule().getDPORule());
			if(!isValid(term)) {
				std::string msg = "Parsing failed for rule '" + r->getName() + "'. " + term.getParsingError();
				throw TermParsingError(std::move(msg));
			}
		}
	}
}

const std::unordered_set<std::shared_ptr<rule::Rule> > &Evaluator::getRuleDatabase() const {
	return database;
}

const std::unordered_set<std::shared_ptr<rule::Rule> > &Evaluator::getProducts() const {
	return products;
}

std::unordered_set<std::shared_ptr<rule::Rule> > Evaluator::eval(const rule::RCExp::Expression &exp) {

	struct PreEvalVisitor : public boost::static_visitor<void> {

		PreEvalVisitor(Evaluator &evaluator) : evaluator(evaluator) { }

		// Nullary/unary
		//----------------------------------------------------------------------

		void operator()(std::shared_ptr<rule::Rule> r) {
			if(evaluator.labelSettings.type == LabelType::Term) {
				const auto &term = get_term(r->getRule().getDPORule());
				if(!isValid(term)) {
					std::string msg = "Parsing failed for rule '" + r->getName() + "'. " + term.getParsingError();
					throw TermParsingError(std::move(msg));
				}
			}
			evaluator.addRule(r);
		}

		void operator()(const rule::RCExp::Union &par) {
			for(const auto &e : par.getExpressions()) e.applyVisitor(*this);
		}

		void operator()(const rule::RCExp::Bind &bind) {
			evaluator.addRule(bind.getGraph()->getGraph().getBindRule());
		}

		void operator()(const rule::RCExp::Id &id) {
			evaluator.addRule(id.getGraph()->getGraph().getIdRule());
		}

		void operator()(const rule::RCExp::Unbind &unbind) {
			evaluator.addRule(unbind.getGraph()->getGraph().getUnbindRule());
		}

		// Binary
		//----------------------------------------------------------------------

		void operator()(const rule::RCExp::ComposeCommon &compose) {
			compose.getFirst().applyVisitor(*this);
			compose.getSecond().applyVisitor(*this);
		}

		void operator()(const rule::RCExp::ComposeParallel &compose) {
			compose.getFirst().applyVisitor(*this);
			compose.getSecond().applyVisitor(*this);
		}

		void operator()(const rule::RCExp::ComposeSub &compose) {
			compose.getFirst().applyVisitor(*this);
			compose.getSecond().applyVisitor(*this);
		}

		void operator()(const rule::RCExp::ComposeSuper &compose) {
			compose.getFirst().applyVisitor(*this);
			compose.getSecond().applyVisitor(*this);
		}
	private:
		Evaluator &evaluator;
	};
	exp.applyVisitor(PreEvalVisitor(*this));
	std::unordered_set<std::shared_ptr<rule::Rule> > result = exp.applyVisitor(EvalVisitor(*this));
	return result;
}

void Evaluator::print() const {
	std::string fileNoExt = IO::RC::Write::pdf(*this);
	IO::post() << "summaryRC \"" << fileNoExt << "\"" << std::endl;
}

const Evaluator::GraphType &Evaluator::getGraph() const {
	return rcg;
}

bool Evaluator::addRule(std::shared_ptr<rule::Rule> r) {
	return database.insert(r).second;
}

void Evaluator::giveProductStatus(std::shared_ptr<rule::Rule> r) {
	products.insert(r);
}

std::shared_ptr<rule::Rule> Evaluator::checkIfNew(lib::Rules::Real *rCand) const {
	for(auto rOther : database) {
		if(lib::Rules::makeIsomorphismPredicate(labelSettings.type, labelSettings.withStereo)(&rOther->getRule(), rCand)) {
			delete rCand;
			return rOther;
		}
	}
	return rule::Rule::makeRule(std::unique_ptr<lib::Rules::Real>(rCand));
}

void Evaluator::suggestComposition(const lib::Rules::Real *rFirst, const lib::Rules::Real *rSecond, const lib::Rules::Real *rResult) {
	Vertex vComp = getVertexFromArgs(rFirst, rSecond);
	Vertex vResult = getVertexFromRule(rResult);
	for(Vertex vOut : asRange(adjacent_vertices(vComp, rcg))) {
		if(vOut == vResult) return;
	}
	std::pair<Edge, bool> pResult = add_edge(vComp, vResult, rcg);
	assert(pResult.second);
	rcg[pResult.first].kind = EdgeKind::Result;
}

Evaluator::Vertex Evaluator::getVertexFromRule(const lib::Rules::Real *r) {
	auto iter = ruleToVertex.find(r);
	if(iter == end(ruleToVertex)) {
		Vertex v = add_vertex(rcg);
		rcg[v].kind = VertexKind::Rule;
		rcg[v].rule = r;
		iter = ruleToVertex.insert(std::make_pair(r, v)).first;
	}
	return iter->second;
}

Evaluator::Vertex Evaluator::getVertexFromArgs(const lib::Rules::Real *rFirst, const lib::Rules::Real *rSecond) {
	Vertex vFirst = getVertexFromRule(rFirst),
			vSecond = getVertexFromRule(rSecond);
	auto iter = argsToVertex.find(std::make_pair(rFirst, rSecond));
	if(iter != end(argsToVertex)) return iter->second;
	Vertex vComp = add_vertex(rcg);
	argsToVertex.insert(std::make_pair(std::make_pair(rFirst, rSecond), vComp));
	rcg[vComp].kind = VertexKind::Composition;
	rcg[vComp].rule = nullptr;
	std::pair<Edge, bool>
			pFirst = add_edge(vFirst, vComp, rcg),
			pSecond = add_edge(vSecond, vComp, rcg);
	assert(pFirst.second);
	assert(pSecond.second);
	rcg[pFirst.first].kind = EdgeKind::First;
	rcg[pSecond.first].kind = EdgeKind::Second;
	return vComp;
}

} // namespace RC
} // namespace lib
} // namespace mod