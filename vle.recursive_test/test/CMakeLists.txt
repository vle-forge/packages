macro (VleBuildTestTemp _testname _cppfile)
  message (" DBG : ${vle.recursive_INCLUDE_DIRS} ")
  link_directories(
    ${VLE_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})
  add_executable(${_testname} ${_cppfile})
  if (CMAKE_VERSION VERSION_LESS 2.8.12)
    include_directories(
      "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};${Boost_INCLUDE_DIRS};"
      "${vle.reader_INCLUDE_DIRS};${vle.recursive_INCLUDE_DIRS}")
  else ()
    target_include_directories(${_testname} PUBLIC
      "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};${Boost_INCLUDE_DIRS};"
      "${vle.reader_INCLUDE_DIRS};${vle.recursive_INCLUDE_DIRS}")
  endif ()
  target_link_libraries(${_testname}
    ${VLE_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${vle.reader_LIBRARIES}
    ${vle.recursive_LIBRARIES})
  add_test(${_testname} ${_testname})
endmacro (VleBuildTestTemp)

VleBuildTestTemp(test_vle_recursive test_vle_recursive.cpp)
VleBuildTestTemp(test_simulatorMPI test_simulatorMPI.cpp)
