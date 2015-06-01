SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(XMLDEPS libxml-2.0)
INCLUDE_DIRECTORIES(${XMLDEPS_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})

#if (VLE_VERSION)
#  include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
#    ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})
#  set(V_LIB vlelib)
#  if (WIN32)
#    link_directories(${VLEDEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
#  endif (WIN32)
#else (VLE_VERSION)
#  message(STATUS "No VLE_VERSION detected, run standalone build")
#  pkg_check_modules(VLEDEPS REQUIRED vle-1.1)
#  include_directories(${VLEDEPS_INCLUDE_DIRS})
#  link_directories(${VLEDEPS_LIBRARY_DIRS})
#  set(V_LIB "vle-1.1")
#endif (VLE_VERSION)

INCLUDE_DIRECTORIES(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
  ${Boost_INCLUDE_DIRS} ${VLE_INCLUDE_DIRS})
LINK_DIRECTORIES(${VLE_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(LOG_SOURCES sim_log.cpp simtab.cpp thread.cpp
  widtoolbar.cpp)

QT4_WRAP_UI(ui_simtab_h simtab.ui)
QT4_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

ADD_LIBRARY(sim_log SHARED ${LOG_SOURCES}
  ${ui_simtab_h} ${ui_widtoolbar_h} )

TARGET_LINK_LIBRARIES(sim_log qwt
  ${QT_LIBRARIES} ${INTL_LIB} -lgvle2-1.1 ${VLE_LIBRARIES})

INSTALL(TARGETS sim_log
  RUNTIME DESTINATION
  plugins/gvle2/simulating LIBRARY DESTINATION plugins/gvle2/simulating)