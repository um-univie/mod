get_filename_component(mod_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)
list(APPEND CMAKE_MODULE_PATH ${mod_CMAKE_DIR})

@libmod_config_dependencies@

include("${mod_CMAKE_DIR}/modTargets.cmake")