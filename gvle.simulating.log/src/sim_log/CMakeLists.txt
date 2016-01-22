SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

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

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GVLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${VLE_LIBRARY_DIRS}
  ${GVLE_LIBRARY_DIRS})

SET(LOG_SOURCES sim_log.cpp simtab.cpp thread.cpp
  widtoolbar.cpp)

QT5_WRAP_UI(ui_simtab_h simtab.ui)
QT5_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

ADD_DEFINITIONS(-DQT_PLUGIN)

ADD_LIBRARY(sim_log SHARED ${LOG_SOURCES}
  ${ui_simtab_h} ${ui_widtoolbar_h} )

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS}
  ${Qt5Gui_DEFINITIONS})

TARGET_LINK_LIBRARIES(sim_log ${INTL_LIB} ${GVLE_LIBRARIES}
  ${VLE_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES})

INSTALL(TARGETS sim_log
  RUNTIME DESTINATION
  plugins/gvle/simulating LIBRARY DESTINATION plugins/gvle/simulating)