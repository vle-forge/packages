function (BuildGenericDt _dynname _cppfiles)
  set (__cppfilesarg "${_cppfiles}")
  link_directories(${VLE_LIBRARY_DIRS})
  add_library(${_dynname} MODULE ${__cppfilesarg})
  if (CMAKE_VERSION VERSION_LESS 2.8.12)
    include_directories(
      "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};"
      "${__include_dirs}")
  else ()
    target_include_directories(${_dynname} PUBLIC
     "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};"
     "${__include_dirs}")
  endif ()
  target_link_libraries(${_dynname} ${__libraries} ${VLE_LIBRARIES}
                        vle.discrete-time)
  install(TARGETS ${_dynname}
                 RUNTIME DESTINATION plugins/simulator
                 LIBRARY DESTINATION plugins/simulator)
endfunction(BuildGenericDt)


BuildGenericDt(GenericSum GenericSum.cpp)
BuildGenericDt(GenericMean GenericMean.cpp)
