#include <mod/py/Common.hpp>

#include <mod/dg/GraphInterface.hpp>
#include <mod/statespace/DynamicDG.hpp>

namespace mod::statespace::Py {

void DynamicDG_doExport() {

py::class_<DynamicDG, std::shared_ptr<DynamicDG>, boost::noncopyable>("DynamicDG", py::no_init)
        .def("apply", &DynamicDG::apply)
        .def("make", &DynamicDG::makeDynamicDG)
        .staticmethod("make")
        ;

}

}
