project(Documentation.AutoStreamForAutoware NONE)

include(Doxygen)

set(DOXYGEN_PROJECT_NAME "AutoStream for Autoware")
set(DOXYGEN_PROJECT_NUMBER ${SDK_VERSION})
add_documentation(TARGET_NAME ${PROJECT_NAME}
  PROJECT_NAME ${DOXYGEN_PROJECT_NAME}
  PROJECT_NUMBER ${DOXYGEN_PROJECT_NUMBER} 
)
