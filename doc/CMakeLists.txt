if(NOT BUILD_DOC)
    return()
endif()

set(output ${CMAKE_CURRENT_BINARY_DIR}/html)
set(doctrees ${CMAKE_CURRENT_BINARY_DIR}/doctrees)
add_custom_target(mod_doc
        COMMAND ${SPHINX} ${CMAKE_CURRENT_SOURCE_DIR}/source -n -b html ${output} -d ${doctrees})
if(TARGET doc)
    add_dependencies(doc mod_doc)
else()
    add_custom_target(doc ALL DEPENDS mod_doc)
endif()
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${output} ${doctrees})
install(DIRECTORY ${output}
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT MOD_doc)
