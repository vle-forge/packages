INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src
  ${GVLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  ${GTKSOURCEVIEW_INCLUDE_DIRS})

LINK_DIRECTORIES(${GVLE_LIBRARY_DIRS}
  ${GTKSOURCEVIEW_LIBRARY_DIRS})

ADD_LIBRARY(modeling-decision SHARED Plugin.hpp Plugin.cpp
  SourceDialog.cpp SourceDialog.hpp Decision.hpp Decision.cpp
  DecisionDrawingArea.hpp DecisionDrawingArea.cpp ActivityDialog.hpp
  ActivityDialog.cpp FactDialog.hpp FactDialog.cpp PredicateDialog.hpp
  PredicateDialog.cpp RuleDialog.hpp RuleDialog.cpp
  PrecedenceConstraintDialog.hpp PrecedenceConstraintDialog.cpp
  OutputFunctionDialog.hpp OutputFunctionDialog.cpp
  AckFunctionDialog.hpp AckFunctionDialog.cpp EditorDialog.hpp
  EditorDialog.cpp Utils.hpp)

SET_TARGET_PROPERTIES(modeling-decision PROPERTIES OUTPUT_NAME
  "Decision")

TARGET_LINK_LIBRARIES(modeling-decision
  ${GVLE_LIBRARIES} decision)

INSTALL(TARGETS modeling-decision RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

INSTALL(FILES decision.glade DESTINATION plugins/gvle/modeling)
