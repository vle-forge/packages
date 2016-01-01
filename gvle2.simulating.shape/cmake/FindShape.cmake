#
# The module defines the following variables:
#  SHAPE_FOUND - the system has ShapeLib
#  SHAPE_INCLUDE_DIR   - where to find shapefil.h
#  SHAPE_INCLUDE_DIRS  - shapelib includes
#  SHAPE_LIBRARY       - where to find the Shape library
#  SHAPE_LIBRARIES     - aditional libraries
#  SHAPE_ROOT_DIR - root dir (ex. /usr/local)

#=============================================================================

find_path ( SHAPE_INCLUDE_DIR
  NAMES shapefil.h
  HINTS ${QT_INCLUDE_DIR}
  PATH_SUFFIXES shapelib libshp
)

set ( SHAPE_INCLUDE_DIRS ${SHAPE_INCLUDE_DIR} )

find_library ( SHAPE_LIBRARY
  NAMES shp shape
  HINTS ${QT_LIBRARY_DIR}
)

find_library ( GLEW_LIBRARY
  NAMES GLEW glew
  HINTS ${QT_LIBRARY_DIR}
)

set ( SHAPE_LIBRARIES ${SHAPE_LIBRARY} ${GLEW_LIBRARY} )

# try to guess root dir from include dir
if ( SHAPE_INCLUDE_DIR )
  string ( REGEX REPLACE "(.*)/include.*" "\\1" SHAPE_ROOT_DIR ${SHAPE_INCLUDE_DIR} )
# try to guess root dir from library dir
elseif ( SHAPE_LIBRARY )
  string ( REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" SHAPE_ROOT_DIR ${SHAPE_LIBRARY} )
endif ()

# handle the QUIETLY and REQUIRED arguments
include ( FindPackageHandleStandardArgs )
if ( CMAKE_VERSION LESS 2.8.3 )
  find_package_handle_standard_args( Shape DEFAULT_MSG SHAPE_LIBRARY SHAPE_INCLUDE_DIR )
else ()
  find_package_handle_standard_args( Shape REQUIRED_VARS SHAPE_LIBRARY SHAPE_INCLUDE_DIR )
endif ()

mark_as_advanced (
  SHAPE_LIBRARY 
  SHAPE_LIBRARIES
  SHAPE_INCLUDE_DIR
  SHAPE_INCLUDE_DIRS
  SHAPE_ROOT_DIR
)
