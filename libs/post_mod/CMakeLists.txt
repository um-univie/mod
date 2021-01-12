if(NOT BUILD_POST_MOD)
    return()
endif()

###########################################################################
# Targets and Artefacts
###########################################################################

install(PROGRAMS bin/mod_post
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT post_mod_run)

if(BUILD_POST_MOD_FMT)
	# commonPreamble.fmt
	add_custom_target(commonPreamble ALL
		pdflatex
		    -interaction=nonstopmode
		    -halt-on-error
		    -ini
		    '&pdflatex \\input{${CMAKE_CURRENT_LIST_DIR}/share/mod/commonPreamble.tex}\\usepackage{${CMAKE_CURRENT_LIST_DIR}/share/mod/mod}\\dump'
		DEPENDS share/mod/commonPreamble.tex share/mod/mod.sty
		BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/share/mod/commonPreamble.fmt
		COMMENT "Pre-compiling Latex preamble (commonPreamble.fmt)")
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/commonPreamble.fmt
		DESTINATION ${CMAKE_INSTALL_DATADIR}/mod
		COMPONENT post_mod_run)
endif()
install(FILES
		share/mod/commonPreamble.tex
		share/mod/figureTemplate.tex
		share/mod/summary.tex
	DESTINATION ${CMAKE_INSTALL_DATADIR}/mod
	COMPONENT post_mod_run)

# Makefile template and Latex package
install(FILES share/mod/mod.mk share/mod/mod.sty
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mod
        COMPONENT post_mod_run)
