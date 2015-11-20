# FindGVLE2.cmake
# ==============
#
# Try to find GVLE2
#
# Copyright 2014-2015 INRA
# Patrick Chabrier <patrick.chabrier@toulouse.inra.fr>
#
# Distributed under the OS-approved BSD License (the "License");
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#
# Once done this will define:
#
#  GVLE2_FOUND            - The system has GVLE2
#  GVLE2_INCLUDE_DIR      - The GVLE2 include directory
#  GVLE2_LIBRARY_DIRS     - Directories containing libraries to link
#  GVLE2_LIBRARIES        - Link these to use shared libraries of GVLE2
#
# CMake variables used by this module:
#
#  GVLE2_DEBUG            - If true, prints debug traces
#                          (default OFF)
#  GVLE2_USING_CMAKE      - If true, on windows, use cmake for finding GVLE2,
#                          else use pkgconfig
#                          (default ON)
#  VLE_BASEPATH_LOCAL    - cmake variable for base path of vle
#                          (default NOT_DEFINED)
#
# Environment variables used by this module:
#
#  VLE_BASEPATH          - environment variable for base path of vle
#
#=============================================================================
#
# Notes :
# ---------
#   FindGVLE2 will try to find GVLE2, VLE and the gtkmm dependencies,
#   finding GVLE2 does not require to call FindVLE before
#
#=============================================================================
#
# Changelog
# ---------
#
# 1.0 Initial version.
#
# Usage
# -----
#
# find_package(GVLE2 REQUIRED)
#
#=============================================================================

#
# Set default behavior of find gvle
#

if (WIN32)
  if (DEFINED GVLE2_USING_CMAKE)
     set (_find_gvle2_using_cmake ${GVLE2_USING_CMAKE})
  else ()
     set (_find_gvle2_using_cmake 1)
  endif ()
else ()
  set (_find_gvle2_using_cmake 0)
endif ()

if (DEFINED GVLE2_DEBUG)
  set (_gvle2_debug  ${GVLE2_DEBUG})
else ()
  set (_gvle2_debug 0)
endif ()

#
# Find GVLE2
#

if (${_find_gvle2_using_cmake})
  find_path(_gvle2_base_include zlib.h PATHS
    $ENV{VLE_BASEPATH}/include
    ${VLE_BASEPATH_LOCAL}/include
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/include")

  find_path(_gvle2_base_bin zlib1.dll PATHS
    $ENV{VLE_BASEPATH}/bin
    ${VLE_BASEPATH_LOCAL}/bin
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/bin"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/bin")

  find_path(_gvle2_base_lib libz.dll.a PATHS
    $ENV{VLE_BASEPATH}/lib
    ${VLE_BASEPATH_LOCAL}/lib
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;Path]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\VLE 1.1.0;]/lib"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\VLE Development Team\\Wow6432Node\\VLE 1.1.0;]/lib")

  if (${_gvle2_debug})
    message (" gvle2_debug _gvle2_base_include ${_gvle2_base_include}")
    message (" gvle2_debug _gvle2_base_bin ${_gvle2_base_bin}")
    message (" gvle2_debug _gvle2_base_lib ${_gvle2_base_lib}")
  endif ()

  if(NOT _gvle2_base_include OR NOT _gvle2_base_bin OR NOT _gvle2_base_lib)
     message (FATAL_ERROR "Missing gvle2 dependencies")
  endif ()

  set(ENV(QTDIR)  ${_gvle2_base_include}/../Qt)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTOMOC ON)

  set(QT_USE_QTXML TRUE)
  set(QT_USE_QTHELP TRUE)

  find_package(Qt4 REQUIRED)
  if (NOT QT_FOUND)
    message(FATAL_ERROR "Qt is required")
  endif (NOT QT_FOUND)
  include(${QT_USE_FILE})

  set(GVLE2_INCLUDE_DIRS
    ${_gvle2_base_include}/vle-1.1; ${_gvle2_base_include};
    ${QT_INCLUDES})

  set (GVLE2_LIBRARY_DIRS
    ${_gvle2_base_bin}; ${_gvle2_base_lib}; ${QT_BINARY_DIR}; ${QT_LIBRARY_DIR})

  set (GVLE2_LIBRARIES
    gvle2-1.1 ${QT_LIBRARIES} intl)
else () # find gvle using pkg-config
  find_package(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(GVLE2 gvle2-1.1)
endif ()
# handle the QUIETLY and REQUIRED arguments and set GVLE2_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GVLE2 REQUIRED_VARS
  GVLE2_INCLUDE_DIRS GVLE2_LIBRARIES)

if (${_gvle_debug})
  message (" gvle2_debug GVLE2_INCLUDE_DIRS ${GVLE2_INCLUDE_DIRS}")
  message (" gvle2_debug GVLE2_LIBRARY_DIRS ${GVLE2_LIBRARY_DIRS}")
  message (" gvle2_debug GVLE2_LIBRARIES ${GVLE2_LIBRARIES}")
endif ()

#mark_as_advanced(GVLE2_INCLUDE_DIRS GVLE2_LIBRARIES GVLE2_LIBRARY_DIRS)
