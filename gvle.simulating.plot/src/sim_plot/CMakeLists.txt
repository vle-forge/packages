set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})#used for QT4_WRAP_UI

#TODO can we compile a standalone version

INCLUDE_DIRECTORIES(${VLE_INCLUDE_DIRS} ${GVLE_INCLUDE_DIRS} 
  ${Boost_INCLUDE_DIRS} ${VLE_BINARY_DIR}/src 
  ${VLE_SOURCE_DIR}/src)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS})

SET(plot_SOURCES plotsignal.cpp sim_plot.cpp simtab.cpp thread.cpp
  widtoolbar.cpp)

QT4_WRAP_UI(ui_simtab_h simtab.ui)
QT4_WRAP_UI(ui_widtoolbar_h widtoolbar.ui)

add_library(sim_plot SHARED
  ${plot_SOURCES}
  ${ui_simtab_h}
  ${ui_widtoolbar_h} )

target_link_libraries(sim_plot
  qwt
  ${QT_LIBRARIES}
  ${INTL_LIB}
  ${VLE_LIBRARIES}
  ${GVLE_LIBRARIES})

INSTALL(TARGETS sim_plot RUNTIME DESTINATION
  plugins/gvle/simulating LIBRARY DESTINATION plugins/gvle/simulating)