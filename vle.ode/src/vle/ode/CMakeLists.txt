INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(ode STATIC
  DifferentialEquation.hpp
  details/DifferentialEquation.cpp
  details/TimeSlicingMethod.cpp
  details/TimeSlicingMethod.hpp
  details/QSS2.hpp
  details/DifferentialEquationImpl.hpp
  )

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(ode PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(ode PROPERTIES OUTPUT_NAME
  "ode-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(ode ${VLE_LIBRARIES})

INSTALL(TARGETS ode ARCHIVE DESTINATION lib)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/ode/Version.hpp)

install(FILES
  ${CMAKE_BINARY_DIR}/src/vle/ode/Version.hpp
  DESTINATION src/vle/ode/)
install(FILES DifferentialEquation.hpp
        DESTINATION src/vle/ode)