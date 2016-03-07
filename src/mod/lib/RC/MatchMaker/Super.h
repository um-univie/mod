#ifndef MOD_LIB_RC_SUPER_H
#define MOD_LIB_RC_SUPER_H

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/lib/Algorithm.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.h>
#include <mod/lib/RC/MatchMaker/MatchMaker.h>

#include <jla_boost/graph/dpo/FilteredGraphProjection.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Super {
	using VertexMapType = jla_boost::GraphMorphism::InvertibleVectorVertexMap<lib::Rule::GraphType, lib::Rule::GraphType>;
public:

	Super(bool allowPartial, bool enforceConstraints) : allowPartial(allowPartial), enforceConstraints(enforceConstraints) { }

	template<typename OuterDomain, typename OuterCodomain, typename MR>
	void makeMatches(const OuterDomain &rFirst, const OuterCodomain &rSecond, MR mr) const {
		if(allowPartial)
			makeMatchesInternal<true>(rFirst, rSecond, mr);
		else
			makeMatchesInternal<false>(rFirst, rSecond, mr);
	}
private:

	template<bool AllowPartial, typename OuterDomain, typename OuterCodomain, typename MR>
	void makeMatchesInternal(const OuterDomain &rFirst, const OuterCodomain &rSecond, MR mr) const {
		if(getConfig().componentSG.verbose.get()) IO::log() << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
		const auto &rFirstRight = get_labelled_right(rFirst.getDPORule());
		const auto &rSecondLeft = get_labelled_left(rSecond.getDPORule());
		//		IO::log() << "rFirstRight:\n";
		//		for(auto v : asRange(vertices(get_graph(rFirstRight)))) {
		//			IO::log() << v << ":";
		//			for(auto vOut : asRange(adjacent_vertices(v, get_graph(rFirstRight))))
		//				IO::log() << " " << vOut;
		//			IO::log() << "\n";
		//		}
		//		IO::log() << "\n";
		//		IO::log() << "rSecondLeft:\n";
		//		for(auto v : asRange(vertices(get_graph(rSecondLeft)))) {
		//			IO::log() << v << ":";
		//			for(auto vOut : asRange(adjacent_vertices(v, get_graph(rSecondLeft))))
		//				IO::log() << " " << vOut;
		//			IO::log() << "\n";
		//		}
		//		IO::log() << std::endl;
		auto mp = makeRuleRuleComponentMonomorphism(rSecondLeft, rFirstRight, enforceConstraints);
		auto mm = makeMultiDimSelector<AllowPartial>(
				get_num_connected_components(rSecondLeft),
				get_num_connected_components(rFirstRight), mp);
		for(const auto &position : mm) {
			auto maybeMap = matchFromPosition(rFirst, rSecond, position);
			if(!maybeMap) continue;
			auto map = *std::move(maybeMap);
			bool continue_ = mr(rFirst, rSecond, std::move(map));
			if(!continue_) break;
		}

		makeMatchesStringOld<AllowPartial>(rFirst, rSecond, mr, mm);
	}
private:
	template<bool AllowPartial, typename Callback, typename MatchMatrix>
	void makeMatchesStringOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, Callback callback, MatchMatrix &mm) const;
	void makeMatchesOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond,
			std::function<bool(const lib::Rule::Real&, const lib::Rule::Real&, const lib::Rule::Real::CoreCoreMap&) > callback) const;
public:
	template<typename Position>
	boost::optional<VertexMapType> matchFromPosition(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const std::vector<Position> &position) const;
private:
	bool allowPartial;
	bool enforceConstraints;
};

template<bool AllowPartial, typename Callback, typename MatchMatrix>
inline void Super::makeMatchesStringOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, Callback callback, MatchMatrix &mm) const {
	auto mmIter = mm.begin();
	auto mmIterEnd = mm.end();
	auto printIter = [&](decltype(mmIter) iter) {
		lib::IO::log() << "(";
		if(iter.owner) {
			for(std::size_t i = 0; i < iter.position.size(); ++i) {
				lib::IO::log() << " " << iter.position[i].host << "/" << (iter.position[i].iterMorphism - mm.morphisms[i][iter.position[i].host].begin());
			}
		}
		lib::IO::log() << " ) of " << rFirst.getRightComponents().size()
				<< ", " << reinterpret_cast<std::ptrdiff_t> (iter.owner);
	};
	{
		//		auto &s = lib::IO::log();
		//		s << "Match matrix new (allowPartial=" << std::boolalpha << this->allowPartial << "):\n";
		//		for(std::size_t h = 0; h < mm.morphisms.front().size(); ++h)
		//			s << std::setw(3) << std::right << h;
		//		s << "\n";
		//		for(std::size_t p = 0; p < mm.morphisms.size(); ++p) {
		//			for(std::size_t h = 0; h < mm.morphisms[p].size(); ++h)
		//				s << std::setw(3) << std::right << mm.morphisms[p][h].size();
		//			s << " : " << p << "\n";
		//		}
		//		lib::IO::log() << "for all matches: ";
		//		lib::IO::log() << "initial = ";
		//		printIter(mmIter);
		//		lib::IO::log() << std::endl;
	}
	makeMatchesOld(rFirst, rSecond, [this, printIter, &callback, &mmIter, mmIterEnd](const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const lib::Rule::Real::CoreCoreMap & match) -> bool {
		auto mapNew = [&]() {
			for(;; ++mmIter) {
				if(mmIter == mmIterEnd) MOD_ABORT;
				auto maybeMapNew = matchFromPosition(rFirst, rSecond, *mmIter);
				if(maybeMapNew) {
					//					lib::IO::log() << "mapNew = ";
					//					printIter(mmIter);
					//					lib::IO::log() << std::endl;
					return *maybeMapNew;
				}
			}
		}();
		const auto &coreFirst = rFirst.getGraph();
				const auto &coreSecond = rSecond.getGraph();
		for(auto vFirst : asRange(vertices(coreFirst))) {
			auto vSecondNew = get_inverse(mapNew, coreSecond, coreFirst, vFirst);
					auto iterOld = match.left.find(vFirst);
			if(iterOld == match.left.end()) {
				if(vSecondNew != lib::Rule::GraphType::null_vertex()) MOD_ABORT;
				} else {
				if(vSecondNew != iterOld->second) MOD_ABORT;
				}
		}
		for(++mmIter; mmIter != mmIterEnd; ++mmIter) {
			auto maybeMapNew = matchFromPosition(rFirst, rSecond, *mmIter);
			if(maybeMapNew) break;
			}
		return true;
	});
	if(mmIter != mmIterEnd) {
		lib::IO::log() << "mmIter:    ";
		printIter(mmIter);
		lib::IO::log() << "\nmmIterEnd: ";
		printIter(mmIterEnd);
		lib::IO::log() << std::endl;
		MOD_ABORT;
	}
}

inline void Super::makeMatchesOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond,
		std::function<bool(const lib::Rule::Real&, const lib::Rule::Real&, const lib::Rule::Real::CoreCoreMap&) > callback) const {
	// Generate the submatches
	const auto &patterns_second_L = rSecond.getLeftComponents();
	const auto &targets_first_R = rFirst.getRightComponents();
	auto labelFirst = rFirst.getStringState().getRight();
	auto labelSecond = rSecond.getStringState().getLeft();
	using Match = GM::VectorVertexMap<Rule::GraphType, Rule::GraphType>;
	using MatchList = std::vector<Match>;
	using TargetMatches = std::vector<MatchList>;
	using MatchMatrix = std::vector<TargetMatches>;
	MatchMatrix matches(patterns_second_L.size(), (TargetMatches(targets_first_R.size())));
	for(unsigned int patternNum = 0; patternNum < patterns_second_L.size(); patternNum++) {
		const auto &pattern = patterns_second_L[patternNum];
		auto patternWrapped = jla_boost::makeFilteredWrapper(pattern);
		for(unsigned int targetNum = 0; targetNum < targets_first_R.size(); targetNum++) {
			const auto &target = targets_first_R[targetNum];
			auto targetWrapped = jla_boost::makeFilteredWrapper(target);
			if(getConfig().componentSG.verbose.get()) IO::log() << "CompSGM:\tpat " << patternNum << "\tgraph " << targetNum << std::endl;

			auto mrStore = GM::makeStoreVertexMap(std::back_inserter(matches[patternNum][targetNum]));
			auto pred = GM::makePropertyPredicate(labelSecond, labelFirst);
			if(enforceConstraints) {
				const auto &constraints = rSecond.getLeftConstraints();
				auto mrCheckConstraints = GraphMorphism::makeCheckConstraints(
						asRange(std::make_pair(constraints.begin(), constraints.end())),
						labelFirst, mrStore);
				//				auto mrPrinter = GraphMorphism::Callback::makePrint(IO::log(), patternWrapped, targetWrapped, mrCheckConstraints);
				GM::VF2Monomorphism()(patternWrapped, targetWrapped,
						GM::makeReinterpreter<jla_boost::FilteredWrapper<Rule::ComponentGraph> >(
						GM::makeToVectorVertexMap(
						GM::makeReinterpreter<Rule::ComponentGraph>(
						GM::makeReinterpreter<Rule::DPOProjection>(
						mrCheckConstraints
						)))),
						pred, pred);
			} else {
				GM::VF2Monomorphism()(patternWrapped, targetWrapped,
						GM::makeReinterpreter<jla_boost::FilteredWrapper<Rule::ComponentGraph> >(
						GM::makeToVectorVertexMap(
						GM::makeReinterpreter<Rule::ComponentGraph>(
						GM::makeReinterpreter<Rule::DPOProjection>(
						mrStore
						)))),
						pred, pred);
			}
		}
	}

	//	{
	//		auto &s = lib::IO::log();
	//		s << "Match matrix old:\n";
	//		for(std::size_t h = 0; h < targets_first_R.size(); ++h)
	//			s << std::setw(3) << std::right << h;
	//		s << "\n";
	//		for(std::size_t p = 0; p < patterns_second_L.size(); ++p) {
	//			for(std::size_t h = 0; h < targets_first_R.size(); ++h)
	//				s << std::setw(3) << std::right << matches[p][h].size();
	//			s << " : " << p << "\n";
	//		}
	//	}

	// Combine matches

	struct State {

		State(unsigned int targetNum, unsigned int matchNum) : targetNum(targetNum), matchNum(matchNum) { }
		unsigned int targetNum;
		unsigned int matchNum;

		static void advance(std::vector<State> &states, const MatchMatrix &mm, unsigned int num) {
			if(num == states.size()) return;
			if(states[num].targetNum == mm[num].size()) {
				states[num].targetNum = states[num].matchNum = 0;
				advance(states, mm, num + 1);
			} else {
				states[num].matchNum++;
				if(states[num].matchNum >= mm[num][states[num].targetNum].size()) {
					states[num].matchNum = 0;
					states[num].targetNum++;
				}
			}
			for(; states[num].targetNum < mm[num].size(); states[num].targetNum++) {
				for(; states[num].matchNum < mm[num][states[num].targetNum].size(); states[num].matchNum++) {
					return;
				}
			}
		}

		static void next(std::vector<State> &states, const MatchMatrix &mm) {
			advance(states, mm, 0);
		}

		static bool atEnd(const std::vector<State> &states, const MatchMatrix &mm) {
			return std::all_of(begin(states), end(states), [&mm](const State & state) {
				return state.targetNum == mm.front().size();
			});
		}

		static bool isPartial(const std::vector<State> &states, const MatchMatrix &mm) {
			return std::any_of(begin(states), end(states), [&mm](const State & state) {
				return state.targetNum == mm.front().size();
			});
		}

		static void print(std::ostream &s, const std::vector<State> &states, const MatchMatrix &mm) {
			s << "(";
			for(unsigned int patNum = 0; patNum < mm.size(); patNum++) {
				s << " " << states[patNum].targetNum << "/" << states[patNum].matchNum;
			}
			s << " )" << std::endl;
		}
	};
	// Initialize state to point to 'all unmatched'
	std::vector<State> states(patterns_second_L.size(), (State(targets_first_R.size(), 0)));
	//	IO::log() << "The match matrix" << std::endl;
	//	{
	//		IO::log() << "  ";
	//		for(unsigned int i = 0; i < matches.front().size(); i++) IO::log() << "\t" << i << ":";
	//		IO::log() << std::endl;
	//		for(unsigned int patNum = 0; patNum < matches.size(); patNum++) {
	//			IO::log() << patNum << ":";
	//			for(unsigned int targetNum = 0; targetNum < matches.front().size(); targetNum++)
	//				IO::log() << "\t" << matches[patNum][targetNum].size();
	//			IO::log() << std::endl;
	//		}
	//	}
	//	IO::log() << "Start of match matrices" << std::endl;
	for(State::next(states, matches); !State::atEnd(states, matches); State::next(states, matches)) {
		//		State::print(IO::log(), states, matches);
		Rule::Real::CoreCoreMap m;
		bool isGood = true;
		for(unsigned int patternNum = 0; patternNum < states.size() && isGood; patternNum++) {
			if(states[patternNum].targetNum == matches[patternNum].size()) continue;
			const MatchList &mList = matches[patternNum][states[patternNum].targetNum];
			assert(states[patternNum].matchNum < mList.size());
			const Match &subMatch = mList[states[patternNum].matchNum];
			if(subMatch.size() != num_vertices(patterns_second_L[patternNum])) {
				IO::log() << subMatch.size() << " != " << num_vertices(patterns_second_L[patternNum]) << std::endl;
			}
			assert(subMatch.size() == num_vertices(patterns_second_L[patternNum]));
			//			{
			//				IO::log() << "from:";
			//				for(unsigned int i = 0; i < subMatch.size(); i++) IO::log() << "\t" << i;
			//				IO::log() << std::endl;
			//
			//				IO::log() << "to:  ";
			//				for(unsigned int i = 0; i < subMatch.size(); i++) {
			//					IO::log() << "\t";
			//					if(subMatch[i] == boost::graph_traits<Rule::Real::ComponentGraph>::null_vertex()) IO::log() << "-";
			//					else IO::log() << get(boost::vertex_index_t(), patterns[pattern], subMatch[i]);
			//				}
			//				IO::log() << std::endl;
			//			}
			for(auto vSecond : asRange(vertices(patterns_second_L[patternNum]))) {
				assert(m.right.find(vSecond) == m.right.end());
				auto vFirst = get(subMatch, rSecond.getGraph(), rFirst.getGraph(), vSecond);
				assert(vFirst != boost::graph_traits<Rule::ComponentGraph>::null_vertex());
				Rule::Real::CoreCoreMap::left_const_iterator iter = m.left.find(vFirst);
				if(iter != m.left.end()) isGood = false;
				else m.insert(Rule::Real::CoreCoreMap::relation(vFirst, vSecond));
			}
		}

		if(!isGood) continue;
		bool isPartial = State::isPartial(states, matches);
		if(!allowPartial && isPartial) continue;
		bool doContinue = callback(rFirst, rSecond, m);
		if(!doContinue) {
			assert(false); // TODO: remove
			break;
		}
	}
	//	IO::log() << "End of match matrices" << std::endl;
}

template<typename Position>
inline boost::optional<Super::VertexMapType>
Super::matchFromPosition(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const std::vector<Position> &position) const {
	const auto &coreFirst = rFirst.getGraph();
	const auto &coreSecond = rSecond.getGraph();
	auto nullFirst = boost::graph_traits<lib::Rule::GraphType>::null_vertex();
	auto nullSecond = boost::graph_traits<lib::Rule::GraphType>::null_vertex();
	VertexMapType map(coreSecond, coreFirst);
	for(std::size_t pId = 0; pId < position.size(); pId++) {
		if(position[pId].disabled) continue;
		if(position[pId].host == rFirst.getNumRightComponents()) {
			if(!allowPartial) MOD_ABORT; // we should have gotten this
			continue;
		}
		const auto &pattern = rSecond.getLeftComponents()[pId];
		assert(position[pId].iterMorphism != position[pId].iterMorphismEnd);
		auto &&morphism = *position[pId].iterMorphism;
		assert(morphism.size() == num_vertices(pattern));
		//	{
		//		IO::log() << "from:";
		//		for(unsigned int i = 0; i < subMatch.size(); i++) IO::log() << "\t" << i;
		//		IO::log() << std::endl;
		//		IO::log() << "to:  ";
		//		for(unsigned int i = 0; i < subMatch.size(); i++) {
		//			IO::log() << "\t";
		//			if(subMatch[i] == boost::graph_traits<Rule::Real::ComponentGraph>::null_vertex()) IO::log() << "-";
		//			else IO::log() << get(boost::vertex_index_t(), patterns[pattern], subMatch[i]);
		//		}
		//		IO::log() << std::endl;
		//	}
		for(auto vSecond : asRange(vertices(pattern))) {
			assert(get(map, coreSecond, coreFirst, vSecond) == nullFirst);
			auto vFirst = get(morphism, coreSecond, coreFirst, vSecond);
			assert(vFirst != nullFirst);
			if(get_inverse(map, coreSecond, coreFirst, vFirst) != nullSecond)
				return boost::none; // the combined match is not injective
			put(map, coreSecond, coreFirst, vSecond, vFirst);
		}
	}
	return map;
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_SUPER_H */
