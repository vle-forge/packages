function(DeclareSimulator targetname name sources)
  include_directories(
    ${VLE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../decision
    ${vle.discrete-time_INCLUDE_DIRS}
    ${vle.extension.decision_INCLUDE_DIRS})

  link_directories(${VLE_LIBRARY_DIRS})

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname}
	vle.discrete-time.decision
	${VLE_LIBRARIES} ${vle.discrete-time_LIBRARIES}
	${vle.extension.decision_LIBRARIES})

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

  install(TARGETS ${targetname}
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator
    ARCHIVE DESTINATION plugins/simulator)
endfunction()

DeclareSimulator(vle.discretime.decision agentDTG AgentDTG.cpp)
