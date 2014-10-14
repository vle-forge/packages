INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${TEMPVAL_INCLUDE_DIRS}
  ${vle.discrete-time_INCLUDE_DIRS} ${vle.extension.decision_INCLUDE_DIRS} 
  ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_LIBRARY(vle.discrete-time.decision STATIC AgentDT.cpp AgentDT.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(vle.discrete-time.decision 
                          PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(vle.discrete-time.decision PROPERTIES OUTPUT_NAME
  "vle-discrete-time-decision-${MODEL_MAJOR}.${MODEL_MINOR}")

message (" DBG comp ${vle.extension.decision_LIBRARIES}")

TARGET_LINK_LIBRARIES(vle.discrete-time.decision ${VLE_LIBRARIES}
    ${TEMPVAL_LIBRARIES} ${vle.discrete-time_LIBRARIES}
    ${vle.extension.decision_LIBRARIES})

INSTALL(TARGETS vle.discrete-time.decision ARCHIVE DESTINATION lib)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/discrete-time/decision/Version.hpp)

install(FILES
  ${CMAKE_BINARY_DIR}/src/vle/discrete-time/decision/Version.hpp
  DESTINATION src/vle/discrete-time/decision/)
install(FILES AgentDT.hpp DESTINATION src/vle/discrete-time/decision/)
