add_subdirectory(dynamics)

#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  #${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

#LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

#FUNCTION(DeclareModuleDynamics name sources)
  #ADD_LIBRARY(${name} SHARED ${sources})
  #TARGET_LINK_LIBRARIES(${name} ${VLE_LIBRARIES} decision)
  #INSTALL(TARGETS ${name}
    #RUNTIME DESTINATION "plugins/simulator"
    #LIBRARY DESTINATION "plugins/simulator")
#ENDFUNCTION(DeclareModuleDynamics)

#FUNCTION(DeclareTest name sources)
  #ADD_EXECUTABLE(${name} ${sources})
  #TARGET_LINK_LIBRARIES(${name} ${VLE_LIBRARIES}
    #${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} decision)
  #ADD_TEST(${name} ${name})
#ENDFUNCTION(DeclareTest name sources)

#DeclareModuleDynamics(DecisionCounter DecisionCounter.cpp)
#DeclareModuleDynamics(DecisionRetarder DecisionRetarder.cpp)
#DeclareModuleDynamics(DecisionStart DecisionStart.cpp)
#DeclareModuleDynamics(OnlyAgent OnlyAgent.cpp)
#DeclareModuleDynamics(OnlyAgentPrecedenceConstraint
  #OnlyAgentPrecedenceConstraint.cpp)
#DeclareModuleDynamics(OnlyAgentWakeUp OnlyAgentWakeUp.cpp)
#DeclareModuleDynamics(SimpleAgent SimpleAgent.cpp)
#DeclareModuleDynamics(ModelSimple ModelSimple.cpp)

#DeclareTest(kb kb.cpp)
#DeclareTest(activity activity.cpp)
#DeclareTest(allenrelation allenrelation.cpp)
#DeclareTest(parser parser.cpp)
#DeclareTest(testdecision testdecision.cpp)
