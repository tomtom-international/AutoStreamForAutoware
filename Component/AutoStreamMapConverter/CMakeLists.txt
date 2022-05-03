project(Component.AutoStreamMapConverter)

find_package(lanelet2_core REQUIRED)
find_package(lanelet2_io REQUIRED)
find_package(lanelet2_projection REQUIRED)

list(APPEND HEADER_FILES 
    include/AutoStreamMapConverter/ArcConverter.hpp
    include/AutoStreamMapConverter/AutoStreamInterface.hpp
    include/AutoStreamMapConverter/ConversionHelpers.hpp
    include/AutoStreamMapConverter/DataTypes.hpp
    include/AutoStreamMapConverter/LaneConverter.hpp
    include/AutoStreamMapConverter/MapConverter.hpp
    include/AutoStreamMapConverter/TrafficSignConverter.hpp
)

list(APPEND SRC_FILES
    src/ArcConverter.cpp
    src/AutoStreamInterface.cpp
    src/ConversionHelpers.cpp
    src/DataTypes.cpp
    src/LaneConverter.cpp
    src/MapConverter.cpp
    src/TrafficSignConverter.cpp
)

add_library(${PROJECT_NAME} SHARED 
    ${HEADER_FILES}
    ${SRC_FILES}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    AutoStreamClientMain
    AutoStreamClient.BoostHttpsClient
    AutoStreamClient.HttpDataUsageLogger
    AutoStreamClient.SqlitePersistentTileCache
    ${lanelet2_core_LIBRARIES}
    ${lanelet2_io_LIBRARIES}
    ${lanelet2_projection_LIBRARIES}
)
