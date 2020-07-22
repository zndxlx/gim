file(GLOB evpp_SRCS *.cc *.h httpc/*.*)

message(STATUS "evpp_SRCS : " ${evpp_SRCS})

set (MACOSX_RPATH ${LIBRARY_OUTPUT_PATH})

include_directories(${PROJECT_SOURCE_DIR})

add_library(evpp_static STATIC ${evpp_SRCS})



