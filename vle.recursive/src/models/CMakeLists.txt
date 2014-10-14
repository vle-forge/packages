
  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src
    ${VLE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(
    ${VLE_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})
  ADD_LIBRARY(SimulatorMPI MODULE "SimulatorMPI.cpp")
  TARGET_LINK_LIBRARIES(SimulatorMPI
    ${VLE_LIBRARIES}
    ${Boost_LIBRARIES}
    vle-recursive)
  INSTALL(TARGETS SimulatorMPI
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

