cmake_minimum_required(VERSION 2.8)

INIT_BIICODE_BLOCK()
#set(BII_LIB_SRC
#	${CMAKE_CURRENT_SOURCE_DIR}/include/ATE.h;
#	${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib.h;
#	${CMAKE_CURRENT_SOURCE_DIR}/include/scriptEngine.h;
#)
#unset(BII_BLOCK_EXES)
ADD_BIICODE_TARGETS()

INCLUDE(biicode/cmake/tools) # Include tools.cmake file from cmake block from the "biicode" user
ACTIVATE_CPP11(INTERFACE ${BII_BLOCK_TARGET})

#Include biicode Boost setup
include(biicode/boost/setup)
bii_find_boost(COMPONENTS system filesystem REQUIRED)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

#set_property(TARGET test PROPERTY CXX_STANDARD 14)