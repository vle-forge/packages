function(DeclareOutput targetname name sources)
  include_directories(
    ${VLE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})

  link_directories(${VLE_LIBRARY_DIRS})

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname} ${VLE_LIBRARIES})

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

  install(TARGETS ${targetname}
    RUNTIME DESTINATION plugins/output
    LIBRARY DESTINATION plugins/output
    ARCHIVE DESTINATION plugins/output)
endfunction()

function(DeclareSimulator targetname name sources)
  include_directories(
    ${VLE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})

  link_directories(${VLE_LIBRARY_DIRS})

  add_library(${targetname} MODULE ${sources})

  target_link_libraries(${targetname} ${VLE_LIBRARIES})

  set_target_properties(${targetname} PROPERTIES
    OUTPUT_NAME ${name}
    COMPILE_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden")

  install(TARGETS ${targetname}
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator
    ARCHIVE DESTINATION plugins/simulator)
endfunction()

DeclareSimulator(pkg-world2d world2d world-2d.cpp)
DeclareSimulator(pkg-world2d-sum world2d-sum Sum.cpp)
DeclareSimulator(pkg-world2d-gen world2d-get Generator.cpp)
DeclareOutput(pkg-world2d-grid world2d-grid grid-output.cpp)
