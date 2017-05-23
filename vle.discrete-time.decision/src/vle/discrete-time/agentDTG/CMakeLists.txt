function(DeclareSimulator targetname name sources)
  include_directories(
    ${VLE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ../decision
    ${vle.discrete-time_INCLUDE_DIRS}
    ${vle.extension.decision_INCLUDE_DIRS})

  link_directories(${VLE_LIBRARY_DIRS} ../decision)

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname}
	libvle-discrete-time-decision-0.1.a
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
