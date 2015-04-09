cmake_minimum_required(VERSION 2.8)

IF(BIICODE)

ADD_BII_TARGETS()

INCLUDE(biicode/cmake/tools) # Include tools.cmake file from cmake block from the "biicode" user
ACTIVATE_CPP11(INTERFACE ${BII_BLOCK_TARGET})

#Include biicode Boost setup
include(biicode/boost/setup)
bii_find_boost(COMPONENTS system filesystem REQUIRED)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

ELSE(BIICODE)

ENDIF(BIICODE)