SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

SET(QMAKE_QUERY_LIBPATH qmake -query QT_INSTALL_LIBS)

EXECUTE_PROCESS(COMMAND ${QMAKE_QUERY_LIBPATH} OUTPUT_VARIABLE
  CMAKE_PREFIX_PATH)

FIND_PACKAGE(Qt5 COMPONENTS Widgets Xml Gui Svg PrintSupport
  Concurrent OpenGL)

IF (NOT Qt5_FOUND)
  MESSAGE(FATAL_ERROR "Qt5 is required")
ENDIF (NOT Qt5_FOUND)
IF (NOT Qt5Widgets_FOUND)
  MESSAGE(FATAL_ERROR "Qt5Widgets is required")
ENDIF (NOT Qt5Widgets_FOUND)
IF (NOT Qt5Xml_FOUND)
  MESSAGE(FATAL_ERROR "Qt5Xml is required")
ENDIF (NOT Qt5Xml_FOUND)
IF (NOT Qt5Gui_FOUND)
  MESSAGE(FATAL_ERROR "Qt5Gui is required")
ENDIF (NOT Qt5Gui_FOUND)
IF (NOT Qt5Svg_FOUND)
  MESSAGE(FATAL_ERROR "Qt5Svg is required")
ENDIF (NOT Qt5Svg_FOUND)
IF (NOT Qt5PrintSupport_FOUND)
  MESSAGE(FATAL_ERROR "Qt5PrintSupport is required")
ENDIF (NOT Qt5PrintSupport_FOUND)
IF (NOT Qt5Concurrent_FOUND)
  MESSAGE(FATAL_ERROR "Qt5Concurrent is required")
ENDIF (NOT Qt5Concurrent_FOUND)
IF (NOT Qt5OpenGL_FOUND)
  MESSAGE(FATALOpenGL_ERROR "Qt5 is required")
ENDIF (NOT Qt5OpenGL_FOUND)

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES}
  ${Qt5Gui_INCLUDES} ${Qt5Svg_INCLUDES} ${Qt5PrintSupport_INCLUDES}
  ${Qt5Concurrent_INCLUDES} ${Qt5OpenGL_INCLUDES})

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS} ${GVLE_INCLUDE_DIRS}
  ${VLE_BINARY_DIR}/src
  ${ext.shapelib_INCLUDE_DIRS} ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS}
  ${GVLE_LIBRARY_DIRS} ${ext.shapelib_LIBRARY_DIRS})

SET(shape_SOURCES sim_spacial.cpp simtab.cpp widtoolbar.cpp
    openglwidget.cpp shaderwidget.cpp shapefile.cpp shapeobject.cpp )

QT5_WRAP_UI(ui_simtab_h simtab.ui)
QT5_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

ADD_LIBRARY(gvleshape SHARED ${shape_SOURCES} ${ui_simtab_h}
  ${ui_widtoolbar_h} openglwidget.h shapefile.h shapeobject.h)

ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS}
  ${Qt5Gui_DEFINITIONS} ${Qt5Svg_DEFINITIONS}
  ${Qt5PrintSupport_DEFINITIONS} ${Qt5Concurrent_DEFINITIONS}
  ${Qt5OpenGL_DEFINITIONS})

TARGET_LINK_LIBRARIES(gvleshape ${ext.shapelib_LIBRARIES} ${INTL_LIB}
  ${GVLE_LIBRARIES} ${VLE_LIBRARIES} ${Qt5Widgets_LIBRARIES}
  ${Qt5Xml_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Svg_LIBRARIES}
  ${Qt5PrintSupport_LIBRARIES} ${Qt5Concurrent_LIBRARIES}
  ${Qt5OpenGL_LIBRARIES})

INSTALL(TARGETS gvleshape RUNTIME DESTINATION
  plugins/gvle/simulating LIBRARY DESTINATION plugins/gvle/simulating)
