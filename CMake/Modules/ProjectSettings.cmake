get_property(INCLUDE_GUARD GLOBAL PROPERTY PROJECTSETTINGS_INCLUDED)
if(INCLUDE_GUARD)
  return()
endif()
set_property(GLOBAL PROPERTY PROJECTSETTINGS_INCLUDED TRUE)

# Set project specific compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(PROJECT_WARNING_FLAGS     "-Wall -Wcast-qual -Werror -Wextra -Wwrite-strings -Wvla  -Wswitch-enum")
  set(PROJECT_C_FLAGS_DEBUG     "${PROJECT_WARNING_FLAGS}")
  set(PROJECT_C_FLAGS_RELEASE   "${PROJECT_WARNING_FLAGS}")
  set(PROJECT_CXX_FLAGS_DEBUG   "${PROJECT_WARNING_FLAGS} -Og")
  set(PROJECT_CXX_FLAGS_RELEASE "${PROJECT_WARNING_FLAGS} -fext-numeric-literals")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
  set(PROJECT_WARNING_FLAGS     "-Wall -Wcast-qual -Werror -Wextra -Wwrite-strings -Wvla  -Wswitch-enum")
  set(PROJECT_C_FLAGS_DEBUG     "${PROJECT_WARNING_FLAGS}")
  set(PROJECT_C_FLAGS_RELEASE   "${PROJECT_WARNING_FLAGS}")
  set(PROJECT_CXX_FLAGS_DEBUG   "${PROJECT_WARNING_FLAGS} -fsanitize=address -fsanitize=signed-integer-overflow -fno-sanitize-recover=signed-integer-overflow -fno-omit-frame-pointer")
  set(PROJECT_CXX_FLAGS_RELEASE "${PROJECT_WARNING_FLAGS} -gline-tables-only")
endif()

set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} ${PROJECT_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${PROJECT_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${PROJECT_CXX_FLAGS_RELEASE}")

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
if(LD_VERSION MATCHES "GNU gold")
  if(NOT LINUX_ARM)
  # Cannot use gold linker for LINUX_ARM due to OpenSSL assembly files generating code that is not supported
  # by the gold linker.
    set(LOCAL_LINKER_FLAGS "-fuse-ld=gold")
  endif()
endif()

set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} ${LOCAL_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LOCAL_LINKER_FLAGS}")

if(APPLE)
  # Add -c option to ranlib; needed for OpenSSL
  set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -c <TARGET>")
endif()
