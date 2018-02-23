find_package(LATEX)
if(PDFLATEX_COMPILER)
  add_custom_command(
    OUTPUT  manual.pdf
    COMMAND ${PDFLATEX_COMPILER}
    ARGS manual.tex
    DEPENDS manual.tex
  )
  file(GLOB CLEAN_FILES *.aux *.log *.lof *.lol *.lot *.out *.toc)
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_FILES}")
endif()
add_custom_target(manual ALL echo DEPENDS manual.pdf)

