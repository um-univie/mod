#ifndef MOD_LIB_RC_SUB_H
#define MOD_LIB_RC_SUB_H

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

struct Sub {
	using VertexMapType = jla_boost::GraphMorphism::InvertibleVectorVertexMap<lib::Rule::GraphType, lib::Rule::GraphType>;
public:

	explicit Sub(bool allowPartial) : allowPartial(allowPartial) { }

	template<typename MR>
	void makeMatches(const Rule::Real &rFirst, const Rule::Real &rSecond, MR mr) const {
		if(allowPartial)
			makeMatchesInternal<true>(rFirst, rSecond, mr);
		else
			makeMatchesInternal<false>(rFirst, rSecond, mr);
	}

private:

	template<bool AllowPartial, typename MR>
	void makeMatchesInternal(const Rule::Real &rFirst, const Rule::Real &rSecond, MR mr) const {
		const auto &rFirstRight = get_labelled_right(rFirst.getDPORule());
		const auto &rSecondLeft = get_labelled_left(rSecond.getDPORule());
		auto mp = makeRuleRuleComponentMonomorphism(rFirstRight, rSecondLeft, false);
		auto mm = makeMultiDimSelector<AllowPartial>(
				get_num_connected_components(rFirstRight),
				get_num_connected_components(rSecondLeft), mp);
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
	const bool allowPartial;
};

template<bool AllowPartial, typename Callback, typename MatchMatrix>
inline void Sub::makeMatchesStringOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, Callback callback, MatchMatrix &mm) const {
	auto mmIter = mm.begin();
	auto mmIterEnd = mm.end();
	auto printIter = [&](decltype(mmIter) iter) {
		lib::IO::log() << "(";
		if(iter.owner) {
			for(std::size_t i = 0; i < iter.position.size(); ++i) {
				lib::IO::log() << " " << iter.position[i].host << "/" << (iter.position[i].iterMorphism - mm.morphisms[i][iter.position[i].host].begin());
			}
		}
		lib::IO::log() << " ) of " << rSecond.getLeftComponents().size()
				<< ", " << reinterpret_cast<std::ptrdiff_t> (iter.owner);
	};
	{ // TODO: not adapted from the Super version
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

inline void Sub::makeMatchesOld(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond,
		std::function<bool(const lib::Rule::Real&, const lib::Rule::Real&, const lib::Rule::Real::CoreCoreMap&) > callback) const {
	namespace GM = jla_boost::GraphMorphism;
	if(getConfig().componentSG.verbose.get()) IO::log() << rFirst.getName() << " -> " << rSecond.getName() << std::endl;

	// Generate the submatches
	auto patterns_first_R = rFirst.getRightComponents();
	auto targets_second_L = rSecond.getLeftComponents();
	auto labelFirst = rFirst.getStringState().getRight();
	auto labelSecond = rSecond.getStringState().getLeft();
	using Match = GM::VectorVertexMap<Rule::GraphType, Rule::GraphType>;
	using MatchList = std::vector<Match>;
	using TargetMatches = std::vector<MatchList>;
	using MatchMatrix = std::vector<TargetMatches>;
	MatchMatrix matches(patterns_first_R.size(), (TargetMatches(targets_second_L.size())));
	for(unsigned int patternNum = 0; patternNum < patterns_first_R.size(); patternNum++) {
		const auto &pattern_first_R = patterns_first_R[patternNum];
		auto patternWrapped_first_R = jla_boost::makeFilteredWrapper(pattern_first_R);
		for(unsigned int targetNum = 0; targetNum < targets_second_L.size(); targetNum++) {
			const auto &target_second_L = targets_second_L[targetNum];
			auto targetWrapped_second_L = jla_boost::makeFilteredWrapper(target_second_L);
			if(getConfig().componentSG.verbose.get()) IO::log() << "CompSGM:\tpat " << patternNum << "\tgraph " << targetNum << std::endl;

			auto mrStore = GM::makeReinterpreter<jla_boost::FilteredWrapper<Rule::ComponentGraph> >(
					GM::makeToVectorVertexMap(
					GM::makeReinterpreter<Rule::ComponentGraph>(
					GM::makeReinterpreter<Rule::DPOProjection>(
					GM::makeStoreVertexMap(std::back_inserter(matches[patternNum][targetNum]))
					))));
			auto pred = GM::makePropertyPredicate(labelFirst, labelSecond);
			GM::VF2Monomorphism()(patternWrapped_first_R, targetWrapped_second_L, mrStore, pred, pred);
		}
	}

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
			s << "  ";
			for(unsigned int i = 0; i < mm.front().size(); i++) s << "\t" << i << ":";
			s << std::endl;
			for(unsigned int patNum = 0; patNum < mm.size(); patNum++) {
				s << patNum << ":";
				for(unsigned int targetNum = 0; targetNum <= mm.front().size(); targetNum++) { // the = is for "no match"
					s << "\t";
					if(targetNum == states[patNum].targetNum) s << states[patNum].matchNum;
					else s << " ";
				}
				s << std::endl;
			}
			s << "isPartial: " << std::boolalpha << isPartial(states, mm) << std::endl;
		}
	};
	// Initialize state to point to 'all unmatched'
	std::vector<State> states(patterns_first_R.size(), (State(targets_second_L.size(), 0)));
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
			assert(subMatch.size() == num_vertices(patterns_first_R[patternNum]));
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
			for(auto vFirst : asRange(vertices(patterns_first_R[patternNum]))) {
				assert(m.left.find(vFirst) == m.left.end());
				auto vSecond = get(subMatch, rFirst.getGraph(), rSecond.getGraph(), vFirst);
				assert(vSecond != boost::graph_traits<Rule::ComponentGraph>::null_vertex());
				Rule::Real::CoreCoreMap::right_const_iterator iter = m.right.find(vSecond);
				if(iter != m.right.end()) isGood = false;
				else m.insert(Rule::Real::CoreCoreMap::relation(vFirst, vSecond));
			}
		}
		if(!isGood) continue;
		bool isPartial = State::isPartial(states, matches);
		if(!allowPartial && isPartial) continue;
		bool doContinue = callback(rFirst, rSecond, m);
		if(!doContinue) break;
	}
	//	IO::log() << "End of match matrices" << std::endl;
}

template<typename Position>
inline boost::optional<Sub::VertexMapType>
Sub::matchFromPosition(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const std::vector<Position> &position) const {
	const auto &coreFirst = rFirst.getGraph();
	const auto &coreSecond = rSecond.getGraph();
	auto nullFirst = boost::graph_traits<lib::Rule::GraphType>::null_vertex();
	auto nullSecond = boost::graph_traits<lib::Rule::GraphType>::null_vertex();
	VertexMapType map(coreSecond, coreFirst);
	for(std::size_t pId = 0; pId < position.size(); pId++) {
		if(position[pId].disabled) continue;
		if(position[pId].host == rSecond.getNumLeftComponents()) {
			if(!allowPartial) MOD_ABORT; // we should have gotten this
			continue;
		}
		const auto &pattern = rFirst.getRightComponents()[pId];
		assert(position[pId].iterMorphism != position[pId].iterMorphismEnd);
		auto &&morphism = *position[pId].iterMorphism;
		assert(morphism.size() == num_vertices(pattern));
		//	{ // TODO: this is not adapted from the Super version
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
		for(auto vFirst : asRange(vertices(pattern))) {
			assert(get_inverse(map, coreSecond, coreFirst, vFirst) == nullSecond);
			auto vSecond = get(morphism, coreFirst, coreSecond, vFirst);
			assert(vSecond != nullSecond);
			if(get(map, coreSecond, coreFirst, vSecond) != nullFirst)
				return boost::none; // the combined match is not injective
			put(map, coreSecond, coreFirst, vSecond, vFirst);
		}
	}
	return map;
}


} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_SUB_H */