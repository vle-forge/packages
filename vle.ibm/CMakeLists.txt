CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(vle.ibm CXX C)
SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
ENABLE_TESTING()

SET(MODEL_MAJOR 0)
SET(MODEL_MINOR 1)
SET(MODEL_PATCH 0)
SET(MODEL_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}.${MODEL_PATCH}")
SET(VLE_NAME "${PROJECT_NAME}-${MODEL_MAJOR}.${MODEL_MINOR}")

##
## Options for compilation of package
##

OPTION(WITH_TEST "will build the test [default: ON]" OFF)
OPTION(WITH_DOC "will compile doc and install it [default: OFF]" OFF)
OPTION(WITH_WARNINGS "will compile with g++ warnings [default: ON]" ON)

if (CMAKE_BUILD_TYPE STREQUAL "")
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
endif ()

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
  if (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  else (NOT WITH_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -fPIC")
  endif (NOT WITH_WARNINGS)
  if ("${CMAKE_BUILD_TYPE}" EQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" EQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")
  endif ()
endif ()


##
## Modules
##

INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)

##
## Check libraries with pkgconfig
##

FIND_PACKAGE(PkgConfig REQUIRED)

##
## Find Lua libs
##
##FIND_PACKAGE(Lua51 REQUIRED)

##
## Check vle
##

set(VLE_DEBUG 0)
set(VLE_ABI_VERSION 1.1)
find_package(VLE REQUIRED)
find_package(GVLE)
set (VleUtils_DIR "${CMAKE_SOURCE_DIR}/cmake")
find_package(VleUtils REQUIRED)
VleCheckDependencies()

option(WITH_GVLE "use the GVLE [default: on]" ON)
if (WITH_GVLE AND GVLE_FOUND)
  set (HAVE_GVLE 1 CACHE INTERNAL "" FORCE)
endif ()

##
## Check VLE's packages
##

set(VleCheckPackage_DIR "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(VleCheckPackage REQUIRED)
SET(VLE_ABI_VERSION 1.1)
VLE_CHECK_PACKAGE(LUA ext.lua)

##
## Subdirectory
##

IF (WITH_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF (WITH_DOC)
ADD_SUBDIRECTORY(exp)
ADD_SUBDIRECTORY(src)

IF (WITH_TEST AND Boost_UNIT_TEST_FRAMEWORK_FOUND)
  ADD_SUBDIRECTORY(test)
ENDIF (WITH_TEST AND Boost_UNIT_TEST_FRAMEWORK_FOUND)

##
## CPack configuration
##

INSTALL(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION .)

INCLUDE(CMakeCPack.cmake)
