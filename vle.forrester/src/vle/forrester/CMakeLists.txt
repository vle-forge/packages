INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${GVLE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  ${GTKSOURCEVIEWMM_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS}
  ${MUPARSER_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${GVLE_LIBRARY_DIRS}
  ${GTKSOURCEVIEWMM_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS}
  ${MUPARSER_LIBRARY_DIRS})

ADD_LIBRARY(Forrester SHARED
  Plugin.hpp Plugin.cpp
  Forrester.hpp Forrester.cpp
  ForresterDrawingArea.hpp ForresterDrawingArea.cpp
  dialogs/SourceDialog.hpp dialogs/SourceDialog.cpp
  dialogs/CompartmentDialog.hpp dialogs/CompartmentDialog.cpp
  dialogs/TimeStepDialog.hpp dialogs/TimeStepDialog.cpp
  dialogs/FlowDialog.hpp dialogs/FlowDialog.cpp
  dialogs/ParameterDialog.hpp dialogs/ParameterDialog.cpp
  dialogs/ExogenousVarDialog.hpp dialogs/ExogenousVarDialog.cpp
  graphicalItems/Parameter.hpp graphicalItems/Parameter.cpp
  graphicalItems/GraphicalItem.hpp graphicalItems/GraphicalItem.cpp
  graphicalItems/ForresterGI.hpp graphicalItems/ForresterGI.cpp
  graphicalItems/Compartment.hpp graphicalItems/Compartment.cpp
  graphicalItems/Flow.hpp graphicalItems/Flow.cpp
  graphicalItems/Anchor.hpp graphicalItems/Anchor.cpp
  graphicalItems/FlowArrow.hpp graphicalItems/FlowArrow.cpp
  graphicalItems/Arrow.hpp graphicalItems/Arrow.cpp
  graphicalItems/DependencyArrow.hpp graphicalItems/DependencyArrow.cpp
  graphicalItems/ExogenousVariable.hpp graphicalItems/ExogenousVariable.cpp
  graphicalItems/Variable.hpp graphicalItems/Variable.cpp
  utilities/ComputeParser.hpp utilities/ComputeParser.cpp
  utilities/Tools.hpp utilities/Tools.cpp
  utilities/Tokenizer.hpp utilities/Tokenizer.cpp)

SET_TARGET_PROPERTIES(Forrester PROPERTIES OUTPUT_NAME
  "Forrester")

TARGET_LINK_LIBRARIES(Forrester
${GVLE_LIBRARIES} ${GTKMM_LIBRARIES} ${MUPARSER_LIBRARIES} ${Boost_LIBRARIES})

INSTALL(TARGETS Forrester RUNTIME DESTINATION
  plugins/gvle/modeling LIBRARY DESTINATION plugins/gvle/modeling)

INSTALL(FILES Forrester.glade DESTINATION plugins/gvle/modeling)
