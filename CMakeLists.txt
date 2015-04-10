cmake_minimum_required(VERSION 3.0)

if(BIICODE)
    add_bii_targets()

    include(biicode/cmake/tools) # Include tools.cmake file from cmake block from the "biicode" user
    activate_cpp11(INTERFACE ${BII_BLOCK_TARGET})

    #Include biicode Boost setup
    include(biicode/boost/setup)
    bii_find_boost(COMPONENTS system filesystem REQUIRED)
    target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})

else(BIICODE)

    ## get cmakepp
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/cmakepp.cmake")
        file(DOWNLOAD "https://github.com/toeb/cmakepp/releases/download/v0.0.7/cmakepp.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cmakepp.cmake")
    endif()
    include("${CMAKE_CURRENT_BINARY_DIR}/cmakepp.cmake")


    ##########
    # Json11 #
    ##########
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Json11)
        message("installing Json11 from github")
        package_source_pull_github(dropbox/json11 dependencies/Json11)
        ans(package_handle)
        if(NOT package_handle)
            message(FATAL_ERROR "could not pull Json11")
        endif()
    endif()



    project(Amanite)

    include(ExternalProject)

    find_package(Boost
      1.47.0        #TODO : what is the minimal version needed ?
      REQUIRED
      COMPONENTS system filesystem)

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


    ##########
    # Json11 #
    ##########
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Json11)

    set(JSON11_SRC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Json11/json11.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Json11/json11.cpp)

    add_executable(Amanite_Test ${JSON11_SRC} tests/test1.cpp)

    add_dependencies(Amanite_Test ChaiScript)
    set_property(TARGET Amanite_Test PROPERTY CXX_STANDARD 14)
    target_link_libraries(Amanite_Test boost_system boost_filesystem pthread dl)

endif(BIICODE)