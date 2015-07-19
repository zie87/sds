CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)
PROJECT(sds)

# The version number
SET(SDS_MAJOR_VERSION 0)
SET(SDS_MINOR_VERSION 1)
SET(SDS_PATCH_VERSION 0)
SET(SDS_VERSION ${SDS_MAJOR_VERSION}.${SDS_MINOR_VERSION}.${SDS_PATCH_VERSION})
# SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# ENABLE_LANGUAGE(CXX)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DBOOST_LOG_DYN_LINK")
ENABLE_LANGUAGE(CXX)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNDEBUG")
ADD_COMPILE_OPTIONS(-std=c++1y) #CMake 2.8.12 or newer

IF ( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
ENDIF()

################################################################################
# FIND BOOST
################################################################################
FIND_PACKAGE( Boost 1.54 COMPONENTS system filesystem program_options thread serialization REQUIRED )

################################################################################
# BUILDING THE SOURCES
################################################################################

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY (src)

