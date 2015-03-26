CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gvle2.experimentalConditions CXX C)
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

OPTION(WITH_TEST "will build the test [default: ON]" ON)
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

set(VLE_DEBUG 0)
find_package(VLE REQUIRED)

##
## Check VLE's packages
##

set (VleCheckPackage_DIR "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(VleCheckPackage REQUIRED)
set (VleCheckAndDeclareGeneratedModels_DIR
  "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(VleCheckAndDeclareGeneratedModels REQUIRED)
SET(VLE_ABI_VERSION 1.1)

##
## Find boost
##

if (WIN32)
  set(Boost_THREADAPI "win32")
endif (WIN32)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost COMPONENTS unit_test_framework date_time thread filesystem system chrono regex program_options )

##
## Find Qt
##

set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
find_package(Qt4 REQUIRED)

##
## Add Intl/Gettext lib
##

if (WIN32)
  set(INTL_LIB "intl")
else (WIN32)
  set(INTL_LIB "")
endif (WIN32)

##
## Generate the doxygen
##

#FIND_PACKAGE(Doxygen)
IF (DOXYGEN)
  SET(DOXYGEN_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
  SET(DOXYGEN_OUTPUT_MODELING_DIR "${PROJECT_BINARY_DIR}/doxygen/modeling")
  SET(DOXYGEN_OUTPUT_SOURCE_DIR "${PROJECT_BINARY_DIR}/doxygen/sources")
  CONFIGURE_FILE("cmake/doxygen-modeling.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")
  CONFIGURE_FILE("cmake/doxygen-sources.conf.in"
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling")
  FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-modeling.conf")

  ADD_CUSTOM_COMMAND(
    OUTPUT "${PROJECT_BINARY_DIR}/doxygen/sources/index.html"
    DEPENDS "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    COMMAND "${DOXYGEN}"
    ARGS "${PROJECT_BINARY_DIR}/doxygen-sources.conf")

  ADD_CUSTOM_TARGET(doc_modeling ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-modeling.conf"
    "${PROJECT_BINARY_DIR}/doxygen/modeling/index.html" VERBATIM)
  ADD_CUSTOM_TARGET(doc_sources ALL DEPENDS
    "${PROJECT_BINARY_DIR}/doxygen-sources.conf"
    "${PROJECT_BINARY_DIR}/doxygen/sources/index.html" VERBATIM)

  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/modeling/html" DESTINATION
    "doc/html/modeling")
  INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/doxygen/sources/html" DESTINATION
    "doc/html/sources")
ENDIF (DOXYGEN)

##
## Subdirectory
##

add_subdirectory(src)

##
## CPack configuration
##

INSTALL(FILES Authors.txt Description.txt License.txt News.txt Readme.txt
  DESTINATION .)

INCLUDE(CMakeCPack.cmake)
