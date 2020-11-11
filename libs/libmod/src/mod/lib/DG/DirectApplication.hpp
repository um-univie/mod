#ifndef MOD_LIB_DG_DIRECT_APPLICATION_HPP
#define MOD_LIB_DG_DIRECT_APPLICATION_HPP

#include <mod/rule/Rule.hpp>
#include <mod/lib/IO/DG.hpp>

namespace mod::lib::DG {

std::vector<std::unique_ptr<Rules::Real>>
directApply(const std::vector<std::shared_ptr<graph::Graph>> &graphs,
               std::shared_ptr<rule::Rule> rOrig, LabelSettings labelSettings,
               int verbosity, IO::Logger& logger);
}

#endif
