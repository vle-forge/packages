INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(differential-equation STATIC
  DifferentialEquation.cpp
  DifferentialEquation.hpp
  TimeSlicingMethod.cpp
  TimeSlicingMethod.hpp
  QSS2.hpp
  PerturbationHandler.cpp
  PerturbationHandler.hpp
  DifferentialEquationImpl.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(differential-equation PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(differential-equation PROPERTIES OUTPUT_NAME
  "differential-equation-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(differential-equation ${VLE_LIBRARIES})

INSTALL(TARGETS differential-equation ARCHIVE DESTINATION lib)

install(FILES DifferentialEquation.hpp DifferentialEquationImpl.hpp
  DllDefines.hpp PerturbationHandler.hpp QSS2.hpp
  TimeSlicingMethod.hpp
  DESTINATION src/vle/extension/differential-equation)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/extension/differential-equation/Version.hpp)

INSTALL(FILES
  ${CMAKE_BINARY_DIR}/src/vle/extension/differential-equation/Version.hpp
  DESTINATION src/vle/extension/differential-equation)
