INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src)

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

MACRO(DeclareOovPlugin name sources)
  ADD_LIBRARY(${name} SHARED ${sources})
  TARGET_LINK_LIBRARIES(${name} ${VLE_LIBRARIES})
  INSTALL(TARGETS ${name}
    RUNTIME DESTINATION plugins/output
    LIBRARY DESTINATION plugins/output)
ENDMACRO(DeclareOovPlugin name sources)

DeclareOovPlugin(dummy "Dummy.cpp;Dummy.hpp")
DeclareOovPlugin(file "File.cpp;File.hpp;FileType.cpp;FileType.hpp")
DeclareOovPlugin(storage "Storage.cpp")
DeclareOovPlugin(console "Console.cpp")
