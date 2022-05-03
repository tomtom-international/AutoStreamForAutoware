project(AutoStreamToLaneletApp)

cmake_minimum_required(VERSION 3.0)

set(HEADER_FILES include/Application/Helpers.hpp)

add_executable(${PROJECT_NAME} src/main.cpp ${HEADER_FILES} src/Helpers.cpp)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Component.AutoStreamMapConverter
)
target_include_directories(${PROJECT_NAME} PRIVATE include)