INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(fsa STATIC DllDefines.hpp FDDevs.cpp FDDevs.hpp FSA.cpp
  FSA.hpp Mealy.cpp Mealy.hpp Moore.cpp Moore.hpp Statechart.cpp
  StatechartDbg.hpp Statechart.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(fsa PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(fsa PROPERTIES OUTPUT_NAME
  "fsa-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(fsa ${VLE_LIBRARIES})

INSTALL(TARGETS fsa ARCHIVE DESTINATION lib)

install(FILES DllDefines.hpp FDDevs.hpp FSA.hpp Mealy.hpp Moore.hpp
  StatechartDbg.hpp Statechart.hpp
  DESTINATION src/vle/extension/fsa)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/extension/fsa/Version.hpp)

INSTALL(FILES
  ${CMAKE_BINARY_DIR}/src/vle/extension/fsa/Version.hpp
  DESTINATION src/vle/extension/fsa)
