get_property(INCLUDE_GUARD GLOBAL PROPERTY DOXYGEN_INCLUDED)
if(INCLUDE_GUARD)
  return()
endif()
set_property(GLOBAL PROPERTY DOXYGEN_INCLUDED TRUE)

###############################################################################
# Add custom target to create doxygen pdf and html documentation files.
#
# Parameter:
#   TARGET_NAME: Name of the custom target
#   PROJECT_NAME: Name of the documentation project. 
#   PROJECT_NUMBER: Documentation revision number.
#   INPUT_FILES: List of Doxygen miscanlous input files.
###############################################################################
function(add_documentation)
  find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen)
  find_package(LATEX)

  if(DOXYGEN_FOUND AND LATEX_FOUND)
    cmake_parse_arguments(_DOXYGEN "" "TARGET_NAME;PROJECT_NAME;PROJECT_NUMBER" "INPUT_FILES" ${ARGN})

    if(NOT _DOXYGEN_TARGET_NAME)
      message(FATAL_ERROR "Please specify a TARGET_NAME to add_documentation().")
    endif()

    if(NOT _DOXYGEN_PROJECT_NAME)
      set(_DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
    endif()

    # Re-run CMake in case doxygen.conf.in is modified 
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS doxygen.conf.in)

    set(DOXYGEN_CONFIG ${PROJECT_BINARY_DIR}/doxygen.conf)
    configure_file(doxygen.conf.in ${DOXYGEN_CONFIG} @ONLY)

    string(REGEX REPLACE "[ :\\]+" "_" OUTPUT_DOXYGEN_PROJECT_NAME ${_DOXYGEN_PROJECT_NAME})

    add_custom_target(${_DOXYGEN_TARGET_NAME} ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
      COMMAND make -C ${PROJECT_BINARY_DIR}/latex
      COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/latex/refman.pdf ${PROJECT_BINARY_DIR}/${OUTPUT_DOXYGEN_PROJECT_NAME}-${_DOXYGEN_PROJECT_NUMBER}.pdf
      DEPENDS ${DOXYGEN_CONFIG} ${_DOXYGEN_INPUT_FILES}
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Generating documentation with Doxygen and Latex"
      VERBATIM
    )
  endif()
endfunction()
