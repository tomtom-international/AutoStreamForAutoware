message(STATUS "building for x86_64 using AutoStream client from ${AUTOSTREAM_CLIENT_SDK_PATH}")

set(ARCH_STRING "x86_64")

set(AUTOSTREAM_CLIENT_INCLUDE_DIR ${AUTOSTREAM_CLIENT_SDK_PATH}/include)
set(AUTOSTREAM_CLIENT_REFERENCE_INCLUDE_DIR ${AUTOSTREAM_CLIENT_SDK_PATH}/reference/include)

add_library(AutoStreamClientMain SHARED IMPORTED GLOBAL)
set_property(TARGET AutoStreamClientMain PROPERTY IMPORTED_LOCATION ${AUTOSTREAM_CLIENT_SDK_PATH}/lib/libTomTom.AutoStream.so)
set_property(TARGET AutoStreamClientMain APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${AUTOSTREAM_CLIENT_INCLUDE_DIR})

add_library(AutoStreamClient.BoostHttpsClient SHARED IMPORTED GLOBAL)
set_property(TARGET AutoStreamClient.BoostHttpsClient PROPERTY IMPORTED_LOCATION ${AUTOSTREAM_CLIENT_SDK_PATH}/reference/lib/libTomTom.AutoStream.BoostHttpsClientV2.so)
set_property(TARGET AutoStreamClient.BoostHttpsClient APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${AUTOSTREAM_CLIENT_REFERENCE_INCLUDE_DIR})

add_library(AutoStreamClient.HttpDataUsageLogger SHARED IMPORTED GLOBAL)
set_property(TARGET AutoStreamClient.HttpDataUsageLogger PROPERTY IMPORTED_LOCATION ${AUTOSTREAM_CLIENT_SDK_PATH}//reference/lib/libTomTom.AutoStream.HttpDataUsageLogger.so)
set_property(TARGET AutoStreamClient.HttpDataUsageLogger APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${AUTOSTREAM_CLIENT_REFERENCE_INCLUDE_DIR})

add_library(AutoStreamClient.SqlitePersistentTileCache SHARED IMPORTED GLOBAL)
set_property(TARGET AutoStreamClient.SqlitePersistentTileCache PROPERTY IMPORTED_LOCATION ${AUTOSTREAM_CLIENT_SDK_PATH}/reference/lib/libTomTom.AutoStream.SqlitePersistentTileCacheV2.so)
set_property(TARGET AutoStreamClient.SqlitePersistentTileCache APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${AUTOSTREAM_CLIENT_REFERENCE_INCLUDE_DIR})