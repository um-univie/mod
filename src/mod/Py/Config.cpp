#include <mod/Py/Common.h>

#include <mod/Config.h>

namespace mod {
namespace Py {

void Config_doExport() {

#define NSIter(rNS, dataNS, tNS)                                                \
			BOOST_PP_SEQ_FOR_EACH_I(SettingIter, ~,                                \
				BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 2, tNS))
#define SettingIter(rSetting, dataSetting, iSetting, tSetting)                  \
			(BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 0, tSetting))

#undef NSIter
#undef SettingIter

	auto configClass = py::class_<mod::Config, boost::noncopyable>("Config", py::no_init);
	{ // Config scope
		py::scope configScope = configClass;

#define NSIter(rNS, dataNS, tNS)                                                                        \
	py::class_<mod::Config:: BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS), boost::noncopyable> \
		("Config" MOD_toString(BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS)), py::no_init)      \
			BOOST_PP_SEQ_FOR_EACH_I(SettingIter,                                                           \
				BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS),                                     \
				BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 2, tNS))                                     \
		;
#define SettingIter(rSetting, dataSetting, iSetting, tSetting)                  \
		.add_property(AddConfig(                                                  \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 0, tSetting),      \
			BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_SETTING_SIZE(), 1, tSetting),      \
			dataSetting                                                            \
		))
#define AddConfig(Type, Name, Class)                                            \
	MOD_toString(Name),                                                          \
	py::make_function([] (mod::Config::Class *c) -> Type {                       \
			return c->Name.get();                                                  \
		},                                                                        \
		py::default_call_policies(),                                              \
		boost::mpl::vector<Type, mod::Config::Class*>()),                         \
	py::make_function([] (mod::Config::Class *c, Type t) -> void {               \
			c->Name.set(t);                                                        \
		},                                                                        \
		py::default_call_policies(),                                              \
		boost::mpl::vector<void, mod::Config::Class*, Type>())

		BOOST_PP_SEQ_FOR_EACH(NSIter, ~, MOD_CONFIG_DATA())

#undef NSIter
#undef SettingIter
#undef AddConfig

		py::list classNames;
#define NSIter(rNS, dataNS, tNS)                                                \
		classNames.append<std::string>("Config" MOD_toString(BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 0, tNS)));
		BOOST_PP_SEQ_FOR_EACH(NSIter, ~, MOD_CONFIG_DATA())
#undef NSIter
				configScope.attr("classNames") = classNames;
	} // Config scope

#define NSIter(rNS, dataNS, tNS)                                                       \
	.def_readonly(MOD_toString(BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 1, tNS)), \
		&mod::Config::BOOST_PP_TUPLE_ELEM(MOD_CONFIG_DATA_NS_SIZE(), 1, tNS))

	configClass
	BOOST_PP_SEQ_FOR_EACH(NSIter, ~, MOD_CONFIG_DATA())
			;

#undef NSIter
#undef SettingIter

	// rst: .. py:class:: Config
	// rst:
	// rst:		This class contains a range of inner classes of config settings. See :doc:`the libMØD documentation</libmod/Config>`.

	// rst: .. py:data:: config
	// rst:
	// rst:		This variable is initialised to the return value of :py:func:`getConfig`, i.e., just use this instead of the method.
	// rst:
	// rst: .. py:method:: getConfig()
	// rst:
	// rst:		:returns: the singleton :cpp:class:`Config` instance used by the library.
	py::def("getConfig", &mod::getConfig, py::return_value_policy<py::reference_existing_object>());
}

} // namespace Py
} // namespace mod
