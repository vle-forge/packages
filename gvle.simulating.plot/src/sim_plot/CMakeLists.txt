set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(QMAKE_QUERY_LIBPATH qmake -query QT_INSTALL_LIBS)

execute_process(COMMAND ${QMAKE_QUERY_LIBPATH} OUTPUT_VARIABLE
  CMAKE_PREFIX_PATH)

find_package(Qt5 COMPONENTS Widgets Xml Gui)
if (NOT Qt5_FOUND)
  message(FATAL_ERROR "Qt5 is required")
endif (NOT Qt5_FOUND)
if (NOT Qt5Widgets_FOUND)
  message(FATAL_ERROR "Qt5Widgets is required")
endif (NOT Qt5Widgets_FOUND)
if (NOT Qt5Xml_FOUND)
  message(FATAL_ERROR "Qt5Xml is required")
endif (NOT Qt5Xml_FOUND)
if (NOT Qt5Gui_FOUND)
  message(FATAL_ERROR "Qt5Gui is required")
endif (NOT Qt5Gui_FOUND)

OPTION(ENABLE_QT5 "enable Qt5 [default: ON]" OFF)

find_package(Qwt)
message("A = ${QWT_INCLUDE_DIR}")
message("B = ${QWT_LIBRARY}")
if (NOT QWT_FOUND)
  message(FATAL_ERROR "Qwt is required")
endif (NOT QWT_FOUND)

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES} ${QWT_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GVLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${VLE_LIBRARY_DIRS}
  ${GVLE_LIBRARY_DIRS})

#include(${QT_USE_FILE})#used for QT4_WRAP_UI??

#TODO can we compile a standalone version

SET(plot_SOURCES plotsignal.cpp sim_plot.cpp simtab.cpp thread.cpp
  widtoolbar.cpp)

QT5_WRAP_UI(ui_simtab_h simtab.ui)
QT5_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

add_library(sim_plot SHARED
  ${plot_SOURCES} ${ui_simtab_h} ${ui_widtoolbar_h})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS}
  ${Qt5Gui_DEFINITIONS})

target_link_libraries(sim_plot
  ${INTL_LIB} ${GVLE_LIBRARIES} ${QWT_LIBRARY}
  ${VLE_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES})

INSTALL(TARGETS sim_plot RUNTIME DESTINATION
  plugins/gvle/simulating LIBRARY DESTINATION plugins/gvle/simulating)