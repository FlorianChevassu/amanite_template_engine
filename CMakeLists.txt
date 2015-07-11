cmake_minimum_required(VERSION 3.0)

project(Amanite)

include(ExternalProject)
 
set(AMANITE_MAJOR_VERSION 0)
set(AMANITE_MINOR_VERSION 1)
set(AMANITE_PATCH_VERSION 0)
set(AMANITE_VERSION
  ${AMANITE_MAJOR_VERSION}.${AMANITE_MINOR_VERSION}.${AMANITE_PATCH_VERSION})

# ================================================
# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/Amanite)
endif()

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# ================================================
# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
	set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# ================================================
# TODO : do not include directory, just add it to AMANITE_INCLUDE_DIRECTORY or something like that.
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(amanite)

# ================================================
# Install dependencies
# ================================================
#
# BOOST 
# 
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost
  1.47.0        #TODO : what is the minimal version needed ?
  REQUIRED
  COMPONENTS system filesystem)	
#include_directories(${Boost_INCLUDE_DIRS})
#target_include_directories(Amanite PUBLIC ${Boost_INCLUDE_DIRS})
#
# ChaiScript 
# 
ExternalProject_Add(
	ChaiScript
	SVN_REPOSITORY https://github.com/ChaiScript/ChaiScript/tags/v5.6.0
	# Force separate output paths for debug and release builds to allow easy
	# identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
			   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=Debug
			   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=Release
	# Disable build and install step
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	# Wrap configure and build steps in a script to log output
	LOG_CONFIGURE ON
	LOG_BUILD ON
)
ExternalProject_Get_Property(ChaiScript SOURCE_DIR)
set(Chaiscript_INCLUDE_DIRS "${SOURCE_DIR}/include")
#include_directories(SYSTEM ${Chaiscript_INCLUDE_DIRS})
#target_include_directories(Amanite PUBLIC ${Chaiscript_INCLUDE_DIRS})
	

# ================================================
# Group files in folders for Visual Studio
set(REG_EXT "[^/]*([.]cpp|[.]h|[.]hpp|[.]txt)$")
foreach(fileName ${AMANITE_SRC} ${AMANITE_CONTEXTS_SRC})
	get_filename_component(fileFolder ${fileName} DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/amanite" "amanite" fileFolder ${fileFolder})
	string(REPLACE "/" "\\\\" fileFolderSourceGroup ${fileFolder})
	source_group(${fileFolderSourceGroup} REGULAR_EXPRESSION ".*/${fileFolder}/${REG_EXT}")
endforeach()


# ================================================
# Export stuff...
#	
# Add all targets to the build-tree export set --> we only have the contexts targets since Amanite is header-only
export(TARGETS JsonContext
  FILE "${PROJECT_BINARY_DIR}/AmaniteTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Amanite)
 
# Create the AmaniteConfig.cmake and AmaniteConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
   
   
set(CONF_LIBRARY_DIRS "${Boost_LIBRARY_DIRS}" "${INSTALL_LIB_DIR}")
set(CONF_LIBRARIES "${Boost_LIBRARIES}" JsonContext)
   
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" "${Boost_INCLUDE_DIRS}" "${Chaiscript_INCLUDE_DIRS}" )
configure_file(AmaniteConfig.cmake.in
  "${PROJECT_BINARY_DIR}/AmaniteConfig.cmake" @ONLY)
  
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${AMANITE_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(AmaniteConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/AmaniteConfig.cmake" @ONLY)
  
# ... for both
configure_file(AmaniteConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/AmaniteConfigVersion.cmake" @ONLY)
 
# Install the AmaniteConfig.cmake and AmaniteConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/AmaniteConfig.cmake"
  "${PROJECT_BINARY_DIR}/AmaniteConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT AmaniteTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)