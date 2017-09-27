 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 set(CMAKE_AUTOMOC ON)

##
## build PlanSimSubpanel
##

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS} ${Qt5Help_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS}
  ${Qt5XmlPatterns_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS})

include_directories(${VLE_BINARY_DIR}/src/pkgs/gvle.default
  ${VLE_SOURCE_DIR}/src ${VLEDEPS_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src/apps/gvle)

add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Gui_DEFINITIONS}
                ${Qt5Xml_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GVLE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${VLE_BINARY_DIR}/src
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS}
  ${GVLE_LIBRARY_DIRS})

QT5_WRAP_UI(simpanelleft_h simpanelleft.ui)
QT5_WRAP_UI(simpanelright_h simpanelright.ui)

set(PlanSimSubpanel_SOURCES_CPP PlanSimSubpanel.cpp vlePlan.cpp
 svgconfig.cpp svgview.cpp ${simpanelleft_h} ${simpanelright_h})

add_library(PlanSimSubpanel SHARED ${PlanSimSubpanel_SOURCES_CPP}
            ${simpanelleft_h} ${simpanelright_h})

target_link_libraries(PlanSimSubpanel ${INTL_LIB} ${GVLE_LIBRARIES}
  ${VLE_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Widgets_LIBRARIES}
  ${Qt5Gui_LIBRARIES} ${Qt5Help_LIBRARIES} ${Qt5Svg_LIBRARIES}
  ${Qt5XmlPattrens_LIBRARIES}
  ${Qt5PrintSupport_LIBRARIES})

INSTALL(TARGETS PlanSimSubpanel RUNTIME DESTINATION
  plugins/gvle/data LIBRARY DESTINATION plugins/gvle/simulating)
