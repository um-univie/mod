#include <boost/python/module.hpp>

namespace mod {
namespace Py {
void Chem_doExport();
void Collections_doExport();
void Config_doExport();
void Derivation_doExport();
void DG_doExport();
void DGStrat_doExport();
void Error_doExport();
void Function_doExport();
void Graph_doExport();
void Misc_doExport();
void RC_doExport();
void Rule_doExport();
void VT_doExport();
} // namespace Py
} // namespace mod

BOOST_PYTHON_MODULE(mod_) {
	mod::Py::Chem_doExport();
	mod::Py::Collections_doExport();
	mod::Py::Config_doExport();
	mod::Py::Derivation_doExport();
	mod::Py::DG_doExport();
	mod::Py::DGStrat_doExport();
	mod::Py::Error_doExport();
	mod::Py::Function_doExport();
	mod::Py::Graph_doExport();
	mod::Py::Misc_doExport();
	mod::Py::RC_doExport();
	mod::Py::Rule_doExport();
}