#ifndef MOD_CONFIG_H
#define MOD_CONFIG_H

// rst: .. _libmod-config:
// rst:
// rst: This file defines configuration options for the library, e.g., for
// rst: changing the number of threads available for the ILP solver, or for
// rst: completely switching to another ILP solver.
// rst: Many of the options control verbosity of algorithms or which data to
// rst: output when printing figures.
// rst: 

#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <limits>
#include <string>

namespace mod {
class Config;

// rst: .. function:: Config &getConfig()
// rst: 
// rst:		:returns: the singleton :cpp:class:`Config` instance used by the library.
// rst:
Config &getConfig();

// rst-class: template<typename T> ConfigSetting
// rst: 
// rst:		Holds a single option of type :cpp:any:`T`.
// rst-class-start:

template<typename T>
struct ConfigSetting {

	ConfigSetting(T value, const std::string &name) : value(value), name(name) { }

	// rst: .. function:: void set(T value)
	// rst:
	// rst:		Sets the configuration value.
	// rst:

	void set(T value) {
		this->value = value;
	};

	// rst: .. function:: T get() const
	// rst:
	// rst:		:returns: The configuration value.
	// rst:

	T get() const {
		return value;
	}

	const std::string &getName() const {
		return name;
	}
private:
	T value;
	const std::string name;
};
// rst-class-end:

// rst-class: Config
// rst:
// rst:		Holds all configuration settings.
// rst:
// rst-class-end:

struct Config {
	Config() = default;
	Config(const Config&) = delete;
	Config &operator=(const Config&) = delete;
	Config(Config&&) = delete;
	Config &operator=(Config&&) = delete;

	// BOOST_PP_SEQ(
	//	BOOST_PP_TUPLE(NamespaceClass, NamespaceName, 
	//		BOOST_PP_SEQ(
	//			BOOST_PP_TUPLE(Type, Name, DefaultValue)
	//		)
	//	)
	// )

#define MOD_CONFIG_DATA()                                                       \
	/* rst: .. todo:: write documentation for all settings */                     \
	((Common, common,                                                             \
		((unsigned int, numThreads, 1))                                             \
	))                                                                            \
	((ComponentSG, componentSG,                                                   \
		((bool, verbose, false))                                                    \
	))                                                                            \
	((DG, dg,                                                                     \
		((bool, calculateVerbose, false))                                           \
		((bool, calculateDetailsVerbose, false))                                    \
		((bool, calculatePredicatesVerbose, false))                    \
		((bool, listUniverse, false))                        \
		((unsigned int, printGraphProduction, std::numeric_limits<unsigned int>::max())) \
		((unsigned int, productLimit, std::numeric_limits<unsigned int>::max()))    \
		((bool, onlyProduceMolecules, false))                                       \
		((bool, putAllProductsInSubset, false))                                     \
		((bool, printVerticesAsPoints, false))                                      \
		((bool, dryDerivationPrinting, false))                                      \
		((bool, ignoreSubset, false))                                               \
		((bool, disableRepeatFixedPointCheck, false))                               \
		((bool, useDotCoords, false))                                               \
		((std::string, graphvizCoordsBegin, ""))                                    \
		((std::string, tikzPictureOption, "scale=\\modDGHyperScale"))               \
		((bool, disallowEmptyParallelStrategies, true))                             \
		((bool, printVertexIds, false))                                             \
	))                                                                            \
	((Graph, graph,                                                               \
		((bool, verboseCache, false))                                               \
		((bool, printSmilesParsingWarnings, true))                                  \
		((bool, appendSmilesClass, false))                                          \
	))                                                                            \
	((IO, io,                                                                     \
		((std::string, dotCoordOptions, ""))                                        \
		((bool, useOpenBabelCoords, true))                                          \
	))                                                                            \
	((OBabel, obabel,                                                             \
		((bool, verbose, false))                                                    \
	))                                                                            \
	((Rule, rule,                                                                 \
		((bool, ignoreConstraintsDuringInversion, false))                           \
		((std::string, changeColour, "purple"))                                     \
		((bool, printChangedEdgesInContext, false))                                 \
	))                                                                            \
	((RC, rc,                                                                     \
		((bool, verbose, false))                                                    \
		((bool, composeConstraints, true))                                          \
		((bool, printMatches, false))                                               \
		((bool, matchesWithIndex, false))                                           \
	))
#define MOD_CONFIG_DATA_NS_SIZE() 3
#define MOD_CONFIG_DATA_SETTING_SIZE() 3

#define MOD_CONFIG_nsIter(rNS, dataNS, tNS)                                       \
	struct BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS) {                 \
		using Self = BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS);          \
		BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS)(const Self&) = delete; \
		Self &operator=(const Self&) = delete;                                        \
		BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS)(Self&&) = delete;      \
		Self &operator=(Self&&) = delete;                                             \
		inline BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS)() :             \
		BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(MOD_CONFIG_settingIterCons,          \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS),                     \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 2, tNS))                     \
		) {}                                                                          \
		BOOST_PP_SEQ_FOR_EACH_I(MOD_CONFIG_settingIter,                               \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS),                     \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 2, tNS))                     \
	} BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 1, tNS);

#define MOD_CONFIG_settingIterCons(rSettting, dataSetting, tSetting)                                \
	BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 1, tSetting)(                                 \
		BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 2, tSetting),                               \
		MOD_toString(dataSetting) "::"                                                                  \
		MOD_toString(BOOST_PP_EXPAND(BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 1, tSetting))) \
	)

#define MOD_CONFIG_settingIter(rSettting, dataSetting, nSetting, tSetting)        \
	ConfigSetting<BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 0, tSetting)> \
		BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 1, tSetting);

#define MOD_toString(s) MOD_toString1(s)
#define MOD_toString1(s) #s

	BOOST_PP_SEQ_FOR_EACH(MOD_CONFIG_nsIter, ~, MOD_CONFIG_DATA())

#undef MOD_CONFIG_settingIterCons
#undef MOD_CONFIG_settingIter
#undef MOD_CONFIG_nsIter
};

} // namespace mod

#endif /* MOD_CONFIG_H */
