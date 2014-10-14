include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/lib)

ADD_LIBRARY(SimpleEquation MODULE SimpleEquation.cpp)
TARGET_LINK_LIBRARIES(SimpleEquation ${VLE_LIBRARIES} recurrence-relation)
INSTALL(TARGETS SimpleEquation
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
