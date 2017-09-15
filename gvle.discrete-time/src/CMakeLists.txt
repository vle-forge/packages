SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

include_directories(${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GVLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS}
  ${GVLE_LIBRARY_DIRS})

SET(discrete-time_SOURCES DiscreteTimePanel.cpp
  DiscreteTimeLeftWidget.cpp
  DiscreteTimeRightWidget.cpp
  vlesmdt.cpp
)

QT5_WRAP_UI(ui_leftWidget_h leftWidget.ui)
QT5_WRAP_UI(ui_rightWidget_h rightWidget.ui)

ADD_DEFINITIONS(-DQT_PLUGIN)

add_library(modeler-discrete-time SHARED ${discrete-time_SOURCES}
  ${ui_leftWidget_h} ${ui_rightWidget_h})

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS}
  ${Qt5Gui_DEFINITIONS})

target_link_libraries(modeler-discrete-time ${INTL_LIB} ${GVLE_LIBRARIES}
  ${VLE_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Xml_LIBRARIES})

INSTALL(TARGETS modeler-discrete-time RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)
