project(cppTool)
cmake_minimum_required(VERSION 3.8.0)

add_library(sTool SHARED)
IF (MSVC)
	string(REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
ENDIF()

file(GLOB srcs *.h *.hh *i *.hpp *.c *.cc *.cpp)

target_sources(sTool PRIVATE ${srcs})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${srcs})