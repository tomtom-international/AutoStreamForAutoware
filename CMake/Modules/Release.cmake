SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/)
message(STATUS "cmake install prefix: ${CMAKE_INSTALL_PREFIX}")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Application/
  DESTINATION Application/
)

install(FILES
  ${CMAKE_SOURCE_DIR}/CMake/Modules/ProjectSettings.cmake
  DESTINATION CMake/Modules/
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Component/
  DESTINATION Component/
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/ DESTINATION docs)

install(FILES CHANGELOG.md LICENSE.md README.md
  DESTINATION .
)

install(FILES ${CMAKE_SOURCE_DIR}/CMakeLists.txt 
  DESTINATION .
)
