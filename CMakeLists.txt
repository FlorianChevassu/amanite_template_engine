cmake_minimum_required(VERSION 3.0)

if(BIICODE)
    add_bii_targets()

    include(biicode/cmake/tools) # Include tools.cmake file from cmake block from the "biicode" user
    activate_cpp11(INTERFACE ${BII_BLOCK_TARGET})

    #Include biicode Boost setup
    include(biicode/boost/setup)
    bii_find_boost(COMPONENTS system filesystem REQUIRED)

    target_include_directories(${BII_BLOCK_TARGET} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Json11")
    target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

else(BIICODE)

    project(Amanite)

    include(ExternalProject)
	set(Boost_USE_STATIC_LIBS   ON)
    find_package(Boost
      1.47.0        #TODO : what is the minimal version needed ?
      REQUIRED
      COMPONENTS system filesystem)
	
	include_directories(${Boost_INCLUDE_DIRS})
	
    include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)

    ##############
    # ChaiScript #
    ############## 
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
    include_directories(SYSTEM ${SOURCE_DIR}/include)
	if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	endif()
	
	add_subdirectory(include)
	
	#Group files in folders for Visual Studio
	set(REG_EXT "[^/]*([.]cpp|[.]h|[.]hpp|[.]txt)$")
	foreach(fileName ${Amanite_SRC})
		get_filename_component(fileFolder ${fileName} DIRECTORY)
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" fileFolder ${fileFolder})
		string(REPLACE "/" "\\\\" fileFolderSourceGroup ${fileFolder})
		source_group(${fileFolderSourceGroup} REGULAR_EXPRESSION ".*/include/${fileFolder}/${REG_EXT}")
	endforeach()
	
    add_executable(Amanite_Test ${Amanite_SRC} tests/test1.cpp)

    add_dependencies(Amanite_Test ChaiScript)
    set_property(TARGET Amanite_Test PROPERTY CXX_STANDARD 14)
    #arget_link_libraries(Amanite_Test system filesystem pthread dl)
	target_link_libraries(Amanite_Test ${Boost_LIBRARIES})
	

endif(BIICODE)