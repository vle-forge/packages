include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CELLDEVS_INCLUDE_DIRS} ${CELLQSS_INCLUDE_DIRS}
  ${DECISION_INCLUDE_DIRS} ${DIFFERENCE_EQU_INCLUDE_DIRS}
  ${DIFFERENTIAL_EQU_INCLUDE_DIRS} ${DSDEVS_INCLUDE_DIRS} ${FSA_INCLUDE_DIRS}
  ${PETRINET_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CELLDEVS_LIBRARY_DIRS}
  ${CELLQSS_LIBRARY_DIRS} ${DECISION_LIBRARY_DIRS}
  ${DIFFERENCE_EQU_LIBRARY_DIRS} ${DIFFERENTIAL_EQU_LIBRARY_DIRS}
  ${DSDEVS_LIBRARY_DIRS} ${FSA_LIBRARY_DIRS} ${PETRINET_LIBRARY_DIRS})

DeclareDifferenceEquationDynamics(EquationA1 A1.cpp;A1.hpp)
DeclareDifferenceEquationDynamics(EquationA2 A2.cpp;A2.hpp)
DeclareDifferenceEquationDynamics(EquationAsynchronousE0 asynchronousE0.cpp)
DeclareDifferenceEquationDynamics(EquationAsynchronousE1 asynchronousE1.cpp)
DeclareDifferenceEquationDynamics(EquationAsynchronousE2 asynchronousE2.cpp)
DeclareDifferenceEquationDynamics(EquationAsynchronousE3 asynchronousE3.cpp)
DeclareFsaDifferenceEquationDynamics(EquationAsynchronousFSA asynchronousFSA.cpp)
DeclareDifferenceEquationDynamics(EquationB1 B1.cpp;B1.hpp)
DeclareDifferenceEquationDynamics(EquationB2 B2.cpp;B2.hpp)
DeclareDifferenceEquationDynamics(EquationB3 B3.cpp)
DeclareDifferenceEquationDynamics(EquationC1 C1.cpp;C1.hpp)
DeclareDifferenceEquationDynamics(EquationC2 C2.cpp;C2.hpp)
DeclareDifferenceEquationDynamics(EquationC3 C3.cpp)
DeclareDifferenceEquationDynamics(EquationD1 D1.cpp;D1.hpp)
DeclareDifferenceEquationDynamics(EquationD2 D2.cpp;D2.hpp)
DeclareDifferenceEquationDynamics(EquationD3 D3.cpp)
DeclareDifferenceEquationDynamics(EquationE1 E1.cpp;E1.hpp)
DeclareDifferenceEquationDynamics(EquationE2 E2.cpp;E2.hpp)
DeclareFsaDifferenceEquationDynamics(EquationEarlyPerturbE0 earlyPerturbE0.cpp)
DeclareFsaDifferenceEquationDynamics(EquationEarlyPerturbE1 earlyPerturbE1.cpp)
DeclareFsaDifferenceEquationDynamics(EquationEarlyPerturbE2 earlyPerturbE2.cpp)
DeclareFsaDifferenceEquationDynamics(EquationEarlyPerturbFSA earlyPerturbFSA.cpp)
DeclareDifferenceEquationDynamics(EquationF1 F1.cpp;F1.hpp)
DeclareDifferenceEquationDynamics(EquationG1 G1.cpp;G1.hpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb1 Perturb1.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb2 Perturb2.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb3 Perturb3.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb4 Perturb4.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb5 Perturb5.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb6 Perturb6.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb7 Perturb7.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb8 Perturb8.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb9 Perturb9.cpp)
DeclareFsaDifferenceEquationDynamics(EquationPerturb10 Perturb10.cpp)
DeclareDifferenceEquationDynamics(EquationSum sum.cpp)
DeclareDifferenceEquationDynamics(confluentNosync_output confluentNosync_output.cpp)
DeclareDifferenceEquationDynamics(confluentNosync_perturb confluentNosync_perturb.cpp)
DeclareDifferenceEquationDynamics(confluentNosync_x confluentNosync_x.cpp)
