INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(difference-equation STATIC Base.cpp Base.hpp DllDefines.hpp
  Generic.cpp GenericDbg.hpp Generic.hpp Multiple.cpp MultipleDbg.hpp
  Multiple.hpp Simple.cpp SimpleDbg.hpp Simple.hpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(difference-equation PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(difference-equation PROPERTIES OUTPUT_NAME
  "difference-equation-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(difference-equation ${VLE_LIBRARIES})

INSTALL(TARGETS difference-equation ARCHIVE DESTINATION lib)

CONFIGURE_FILE(Version.hpp.in
  ${CMAKE_BINARY_DIR}/src/vle/extension/difference-equation/Version.hpp)

INSTALL(FILES
  ${CMAKE_BINARY_DIR}/src/vle/extension/difference-equation/Version.hpp
  DESTINATION src/vle/extension/difference-equation)
