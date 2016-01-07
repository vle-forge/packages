set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})#used for QT4_WRAP_UI

#TODO can we compile a standalone version

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS} ${GVLE_INCLUDE_DIRS} 
  ${Boost_INCLUDE_DIRS} ${VLE_BINARY_DIR}/src ${ext.shapelib_INCLUDE_DIRS}
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS}
  ${ext.shapelib_LIBRARY_DIRS})


include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

SET(shape_SOURCES sim_spacial.cpp simtab.cpp widtoolbar.cpp
    openglwidget.cpp shaderwidget.cpp
    shapefile.cpp shapeobject.cpp )

QT4_WRAP_UI(ui_simtab_h simtab.ui)
QT4_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

add_library(gvleshape SHARED ${shape_SOURCES}
  ${ui_simtab_h} ${ui_widtoolbar_h} openglwidget.h shapefile.h shapeobject.h)

target_link_libraries(gvleshape ${ext.shapelib_LIBRARIES} ${QT_LIBRARIES} ${INTL_LIB})

INSTALL(TARGETS gvleshape RUNTIME DESTINATION
  plugins/gvle/simulating LIBRARY DESTINATION plugins/gvle/simulating)