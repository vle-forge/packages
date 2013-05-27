INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${GVLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

ADD_LIBRARY(modeling-fsa-statechart SHARED Plugin.hpp Plugin.cpp
  SourceDialog.hpp SourceDialog.cpp Statechart.hpp Statechart.cpp
  StatechartDrawingArea.hpp StatechartDrawingArea.cpp StateDialog.hpp
  StateDialog.cpp TimeStepDialog.hpp TimeStepDialog.cpp
  TransitionDialog.hpp TransitionDialog.cpp)

SET_TARGET_PROPERTIES(modeling-fsa-statechart PROPERTIES OUTPUT_NAME
  "Statechart")

TARGET_LINK_LIBRARIES(modeling-fsa-statechart
${GVLE_LIBRARIES} ${Boost_LIBRARIES})

INSTALL(TARGETS modeling-fsa-statechart RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

INSTALL(FILES FSA.glade DESTINATION plugins/gvle/modeling)