INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(decision STATIC Activities.cpp Activities.hpp Activity.cpp
  Activity.hpp Agent.cpp Agent.hpp Facts.hpp KnowledgeBase.cpp
  KnowledgeBase.hpp Library.cpp Library.hpp Plan.cpp Plan.hpp
  PrecedenceConstraint.cpp PrecedenceConstraint.hpp PrecedencesGraph.cpp
  PrecedencesGraph.hpp Predicates.cpp Predicates.hpp Rule.hpp Rules.cpp
  Rules.hpp Table.hpp Version.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(decision PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(decision PROPERTIES OUTPUT_NAME
  "decision-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(decision ${VLE_LIBRARIES} ${Boost_LIBRARIES})

INSTALL(TARGETS decision ARCHIVE DESTINATION lib)

install(FILES Activities.hpp Activity.hpp Agent.hpp
  Facts.hpp KnowledgeBase.hpp Library.hpp Plan.hpp
  PrecedenceConstraint.hpp PrecedencesGraph.hpp Predicates.hpp
  Rule.hpp Rules.hpp Table.hpp
  DESTINATION src/vle/extension/decision)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/extension/decision/Version.hpp)

INSTALL(FILES
  ${CMAKE_BINARY_DIR}/src/vle/extension/decision/Version.hpp
  DESTINATION src/vle/extension/decision)
