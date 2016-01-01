set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
find_package(PkgConfig REQUIRED)
pkg_check_modules(XMLDEPS libxml-2.0)
include_directories(${XMLDEPS_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})

if (VLE_VERSION)
  include_directories(${VLE_BINARY_DIR}/src ${VLE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS} ${VLEDEPS_INCLUDE_DIRS})
  set(V_LIB vlelib)
  if (WIN32)
    link_directories(${VLEDEPS_LIBRARY_DIRS} ${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
  endif (WIN32)
else (VLE_VERSION)
  message(STATUS "No VLE_VERSION detected, run standalone build")
  pkg_check_modules(VLEDEPS REQUIRED vle-1.1)
  include_directories(${VLEDEPS_INCLUDE_DIRS})
  link_directories(${VLEDEPS_LIBRARY_DIRS} ${VLEDEPS_LIBRARY_DIRS})
  set(V_LIB "vle-1.1")
endif (VLE_VERSION)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

SET(plot_SOURCES plotsignal.cpp sim_plot.cpp simtab.cpp thread.cpp
  widtoolbar.cpp)

QT4_WRAP_UI(ui_simtab_h simtab.ui)
QT4_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${VLE_INCLUDE_DIRS}
  ${GVLE2_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${GVLE2_LIBRARY_DIRS})

add_library(sim_plot SHARED
  ${plot_SOURCES}
  ${ui_simtab_h}
  ${ui_widtoolbar_h} )

message("alors???:${GVLE2_LIBRARIES}")


target_link_libraries(sim_plot
  qwt
  ${QT_LIBRARIES}
  ${INTL_LIB}
  ${VLE_LIBRARIES}
  ${GVLE2_LIBRARIES})

INSTALL(TARGETS sim_plot RUNTIME DESTINATION
  plugins/gvle2/simulating LIBRARY DESTINATION plugins/gvle2/simulating)