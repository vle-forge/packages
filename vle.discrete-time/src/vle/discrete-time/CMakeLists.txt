INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(vle.discrete-time STATIC DiscreteTimeDyn.cpp TemporalValues.cpp
               DiscreteTimeDyn.hpp TemporalValues.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(vle.discrete-time PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(vle.discrete-time PROPERTIES OUTPUT_NAME
  "vle-discrete-time-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(vle.discrete-time ${VLE_LIBRARIES})

INSTALL(TARGETS vle.discrete-time ARCHIVE DESTINATION lib)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/discrete-time/Version.hpp)

install(FILES
  ${CMAKE_BINARY_DIR}/src/vle/discrete-time/Version.hpp
  DESTINATION src/vle/discrete-time/)
install(FILES DiscreteTimeDyn.hpp TemporalValues.hpp 
        DESTINATION src/vle/discrete-time)
