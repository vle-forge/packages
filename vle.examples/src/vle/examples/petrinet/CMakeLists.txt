include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CELLDEVS_INCLUDE_DIRS} ${CELLQSS_INCLUDE_DIRS}
  ${DECISION_INCLUDE_DIRS} ${DIFFERENCE_EQU_INCLUDE_DIRS}
  ${DIFFERENTIAL_EQU_INCLUDE_DIRS} ${DSDEVS_INCLUDE_DIRS} ${FSA_INCLUDE_DIRS}
  ${PETRINET_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CELLDEVS_LIBRARY_DIRS}
  ${CELLQSS_LIBRARY_DIRS} ${DECISION_LIBRARY_DIRS}
  ${DIFFERENCE_EQU_LIBRARY_DIRS} ${DIFFERENTIAL_EQU_LIBRARY_DIRS}
  ${DSDEVS_LIBRARY_DIRS} ${FSA_LIBRARY_DIRS} ${PETRINET_LIBRARY_DIRS})

DeclarePetrinetDynamics(PetrinetBeep PetrinetBeep.cpp)
DeclarePetrinetDynamics(PetrinetConflict PetrinetConflict.cpp)
DeclarePetrinetDynamics(PetrinetCounter PetrinetCounter.cpp)
DeclarePetrinetDynamics(PetrinetInOut PetrinetInOut.cpp)
DeclarePetrinetDynamics(PetrinetMeteo PetrinetMeteo.cpp)
DeclarePetrinetDynamics(PetrinetOrdinary PetrinetOrdinary.cpp)
DeclarePetrinetDynamics(PetrinetTrigger PetrinetTrigger.cpp)
