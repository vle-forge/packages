include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CELLDEVS_INCLUDE_DIRS} ${CELLQSS_INCLUDE_DIRS}
  ${DECISION_INCLUDE_DIRS} ${DIFFERENCE_EQU_INCLUDE_DIRS}
  ${DIFFERENTIAL_EQU_INCLUDE_DIRS} ${DSDEVS_INCLUDE_DIRS} ${FSA_INCLUDE_DIRS}
  ${PETRINET_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CELLDEVS_LIBRARY_DIRS}
  ${CELLQSS_LIBRARY_DIRS} ${DECISION_LIBRARY_DIRS}
  ${DIFFERENCE_EQU_LIBRARY_DIRS} ${DIFFERENTIAL_EQU_LIBRARY_DIRS}
  ${DSDEVS_LIBRARY_DIRS} ${FSA_LIBRARY_DIRS} ${PETRINET_LIBRARY_DIRS})

DeclareFsaDynamics(FDDevs1 FDDevs1.cpp)
DeclareFsaDynamics(FDDevs2 FDDevs2.cpp)
DeclareFsaDynamics(FsaGenerator Generator.cpp)
DeclareFsaDifferenceEquationDynamics(LinearEqLin LinearEqLin.cpp)
DeclareFsaDifferenceEquationDynamics(LinearGenLin LinearGenLin.cpp)
DeclareFsaDynamics(Mealy1 Mealy1.cpp)
DeclareFsaDynamics(MealyCounter1 MealyCounter1.cpp)
DeclareFsaDynamics(Meteo Meteo.cpp)
DeclareFsaDynamics(Moore1 Moore1.cpp)
DeclareFsaDynamics(MooreCounter2 MooreCounter2.cpp)
DeclareFsaDifferenceEquationDynamics(Stage Stage.cpp;Stage.hpp)
DeclareFsaDynamics(Statechart1 Statechart1.cpp)
DeclareFsaDynamics(Statechart2 Statechart2.cpp)
DeclareFsaDynamics(Statechart3 Statechart3.cpp)
DeclareFsaDynamics(Statechart4 Statechart4.cpp)
DeclareFsaDynamics(Statechart5 Statechart5.cpp)
DeclareFsaDynamics(Statechart6 Statechart6.cpp)
DeclareFsaDynamics(Statechart7 Statechart7.cpp)
DeclareFsaDynamics(Statechart8 Statechart8.cpp)
DeclareFsaDynamics(Statechart9 Statechart9.cpp)
DeclareFsaDynamics(StatechartListener StatechartListener.cpp)
DeclareFsaDynamics(StatechartMultipleSend StatechartMultipleSend.cpp)
DeclareFsaDifferenceEquationDynamics(StatechartThreshold StatechartThreshold.cpp)
