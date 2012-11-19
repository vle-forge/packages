INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${GVLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS})

ADD_LIBRARY(modeling-DE-simple SHARED Simple.hpp Simple.cpp Plugin.hpp
  Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp
  SourceDialog.cpp NameValue.hpp NameValue.cpp TimeStep.hpp
  TimeStep.cpp Mapping.hpp Mapping.cpp)

SET_TARGET_PROPERTIES(modeling-DE-simple PROPERTIES OUTPUT_NAME
  "DifferenceEquationSimple")

TARGET_LINK_LIBRARIES(modeling-DE-simple ${VLE_LIBRARIES}
  ${GVLE_LIBRARIES} ${GTKSOURCEVIEWMM_LIBRARIES})

INSTALL(TARGETS modeling-DE-simple RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

ADD_LIBRARY(modeling-DE-generic SHARED Generic.hpp Generic.cpp
  Plugin.hpp Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp
  SourceDialog.cpp NameValue.hpp NameValue.cpp TimeStep.hpp
  TimeStep.cpp Mapping.hpp Mapping.cpp)

SET_TARGET_PROPERTIES(modeling-DE-generic PROPERTIES OUTPUT_NAME
  "DifferenceEquationGeneric")

TARGET_LINK_LIBRARIES(modeling-DE-generic ${VLE_LIBRARIES}
  ${GVLE_LIBRARIES} ${GTKSOURCEVIEWMM_LIBRARIES})

INSTALL(TARGETS modeling-DE-generic RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

ADD_LIBRARY(modeling-DE-multiple SHARED Multiple.hpp Multiple.cpp
  Plugin.hpp Plugin.cpp Parameters.hpp Parameters.cpp SourceDialog.hpp
  SourceDialog.cpp Mapping.hpp Mapping.cpp TimeStep.hpp TimeStep.cpp
  Variables.hpp Variables.cpp)

SET_TARGET_PROPERTIES(modeling-DE-multiple PROPERTIES OUTPUT_NAME
  "DifferenceEquationMultiple")

TARGET_LINK_LIBRARIES(modeling-DE-multiple ${VLE_LIBRARIES}
  ${GVLE_LIBRARIES} ${GTKSOURCEVIEWMM_LIBRARIES})

INSTALL(TARGETS modeling-DE-multiple RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

INSTALL(FILES DifferenceEquation.glade DESTINATION
  plugins/gvle/modeling)
