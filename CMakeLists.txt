cmake_minimum_required(VERSION 3.10.0)

# General location of the CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake/Modules)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "\$ORIGIN" "\$ORIGIN/../lib")

project(AutoStreamForAutoware)

include(ProjectSettings)

add_subdirectory(Component)
add_subdirectory(Application)  

if (EXISTS ${CMAKE_SOURCE_DIR}/CMake/Modules/Release.cmake)
    include(Release)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/CMake/Modules/AutoStream.cmake)
    include(AutoStream)
endif()
