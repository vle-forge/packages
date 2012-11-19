INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

FUNCTION(DeclareTest name sources)
  ADD_EXECUTABLE(${name} ${sources})
  TARGET_LINK_LIBRARIES(${name} ${VLE_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} decision)
  ADD_TEST(${name} ${name})
ENDFUNCTION(DeclareTest name sources)

DeclareTest(kb kb.cpp)
DeclareTest(activity activity.cpp)
DeclareTest(allenrelation allenrelation.cpp)
DeclareTest(parser parser.cpp)
DeclareTest(ss ss.cpp)
