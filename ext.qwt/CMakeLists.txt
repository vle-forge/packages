CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ext.qwt CXX C)
ENABLE_TESTING()

SET(MODEL_MAJOR 0)
SET(MODEL_MINOR 1)
SET(MODEL_PATCH 0)
SET(MODEL_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}.${MODEL_PATCH}")
SET(VLE_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}")

##
## Options for compilation of package
##

OPTION(WITH_TEST "will build the test [default: ON]" ON)
OPTION(WITH_DOC "will compile doc and install it [default: OFF]" OFF)
OPTION(WITH_WARNINGS "will compile with g++ warnings [default: ON]" ON)

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

IF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
  IF (NOT WITH_WARNINGS)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  ELSE (NOT WITH_WARNINGS)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  ENDIF (NOT WITH_WARNINGS)
  IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  ENDIF ()
ENDIF ()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH})

##
## Modules
##

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)

##
## Check VLE and package dependencies
##

SET(VLE_DEBUG 0)
SET(VLE_ABI_VERSION 1.3)
FIND_PACKAGE(VLE REQUIRED)
SET (VleUtils_DIR "${VLE_SHARE_DIR}/cmake")
FIND_PACKAGE(VleUtils REQUIRED)
VleCheckDependencies()

##
## Subdirectory
##

IF(WITH_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF(WITH_DOC)
ADD_SUBDIRECTORY(src)

##
## CPack configuration
##

INSTALL(FILES Authors.txt Description.txt License.txt News.txt
  Readme.txt CHANGES-6.1 COPYING README INSTALL DESTINATION .)

INCLUDE(CMakeCPack.cmake)
