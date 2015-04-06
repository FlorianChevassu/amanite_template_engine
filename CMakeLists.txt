PROJECT( source )
cmake_minimum_required(VERSION 3.0)

# Set project layout variables
SET(BII_PROJECT_ROOT "/home/florian/Amanite/source")
SET(BII_BLOCKS_DIR blocks)
SET(BII_DEPS_DIR deps)
SET(BII_LIB_DIR lib)
SET(BII_CMAKE_DIR )
SET(BII_BIN_DIR bin)
set(CMAKE_MODULE_PATH "${BII_PROJECT_ROOT}/blocks"
					  "${BII_PROJECT_ROOT}/deps")

# inclusion of general biicode macros, biicode.cmake 
INCLUDE(biicode.cmake) 

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BII_PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BII_PROJECT_ROOT}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BII_PROJECT_ROOT}/bin)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BII_PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BII_PROJECT_ROOT}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BII_PROJECT_ROOT}/lib)

# Including /blocks and /deps directories
INCLUDE_DIRECTORIES(${BII_PROJECT_ROOT}/blocks)
INCLUDE_DIRECTORIES(${BII_PROJECT_ROOT}/deps)

ADD_DEFINITIONS(-DBIICODE)
SET(BIICODE_ENV_DIR "/home/florian/.biicode")

#artifact to allow upstream configurations
BII_PREBUILD_STEP(deps/lasote/stringutils)
BII_PREBUILD_STEP(blocks/lefticus/ChaiScript)
BII_PREBUILD_STEP(deps/lasote/json11)
BII_PREBUILD_STEP(blocks/FlorianChevassu/AmaniteTemplateEngine)

# Enable tests with CTest
enable_testing()
# Creating a custom target which will bring associated
# all the test targets
add_custom_target(biitest)

# Inclusion of the blocks affected by the build
BII_INCLUDE_BLOCK(deps/lasote/stringutils)
BII_INCLUDE_BLOCK(blocks/lefticus/ChaiScript)
BII_INCLUDE_BLOCK(deps/lasote/json11)
BII_INCLUDE_BLOCK(blocks/FlorianChevassu/AmaniteTemplateEngine)
