if(NOT BUILD_EPIM)
    return()
endif()

install(DIRECTORY lib/mod
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT epim_run
        FILES_MATCHING PATTERN "*.py")