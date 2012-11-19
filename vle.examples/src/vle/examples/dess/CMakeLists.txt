include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CELLDEVS_INCLUDE_DIRS} ${CELLQSS_INCLUDE_DIRS}
  ${DECISION_INCLUDE_DIRS} ${DIFFERENCE_EQU_INCLUDE_DIRS}
  ${DIFFERENTIAL_EQU_INCLUDE_DIRS} ${DSDEVS_INCLUDE_DIRS} ${FSA_INCLUDE_DIRS}
  ${PETRINET_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CELLDEVS_LIBRARY_DIRS}
  ${CELLQSS_LIBRARY_DIRS} ${DECISION_LIBRARY_DIRS}
  ${DIFFERENCE_EQU_LIBRARY_DIRS} ${DIFFERENTIAL_EQU_LIBRARY_DIRS}
  ${DSDEVS_LIBRARY_DIRS} ${FSA_LIBRARY_DIRS} ${PETRINET_LIBRARY_DIRS})

DeclareDifferentialEquationDynamics(DessCounter Counter.cpp;Counter.hpp)
DeclareDifferentialEquationDynamics(DessE E.cpp;E.hpp)
DeclareDifferentialEquationDynamics(DessI2 I2.cpp;I2.hpp)
DeclareDifferentialEquationDynamics(DessI3 I3.cpp;I3.hpp)
DeclareDifferentialEquationDynamics(DessI I.cpp;I.hpp)
DeclareDifferentialEquationDynamics(DessR2 R2.cpp;R2.hpp)
DeclareDifferentialEquationDynamics(DessR3 R3.cpp;R3.hpp)
DeclareDifferentialEquationDynamics(DessR R.cpp;R.hpp)
DeclareDifferentialEquationDynamics(DessS2 S2.cpp;S2.hpp)
DeclareDifferentialEquationDynamics(DessS3 S3.cpp;S3.hpp)
DeclareDifferentialEquationDynamics(DessS S.cpp;S.hpp)
DeclareDifferentialEquationDynamics(DessSeir seir.cpp;seir.hpp)
DeclareDifferentialEquationDynamics(DessSir sir.cpp;sir.hpp)
