INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${GVLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

ADD_LIBRARY(modeling-petrinet MODULE ArcDialog.cpp ArcDialog.hpp
  PetriNet.cpp PetriNetDrawingArea.cpp PetriNetDrawingArea.hpp
  PetriNet.hpp PlaceDialog.cpp PlaceDialog.hpp Plugin.cpp Plugin.hpp
  TransitionDialog.cpp TransitionDialog.hpp)

SET_TARGET_PROPERTIES(modeling-petrinet PROPERTIES OUTPUT_NAME
"Petrinet")

TARGET_LINK_LIBRARIES(modeling-petrinet ${VLE_LIBRARIES}
  ${GVLE_LIBRARIES} ${GTKSOURCEVIEWMM_LIBRARIES})

INSTALL(TARGETS modeling-petrinet RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

INSTALL(FILES PetriNet.glade DESTINATION plugins/gvle/modeling)
