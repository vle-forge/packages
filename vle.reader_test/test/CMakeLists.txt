macro (VleBuildTestTemp _testname _cppfile)
  link_directories(
    ${VLE_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})
  add_executable(${_testname} ${_cppfile})
  target_include_directories(${_testname} PUBLIC
    "${CMAKE_SOURCE_DIR}/src;${VLE_INCLUDE_DIRS};${Boost_INCLUDE_DIRS};"
     "${vle.reader_INCLUDE_DIRS}")
  target_link_libraries(${_testname}
    ${VLE_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${vle.reader_LIBRARIES})
  add_test(${_testname} ${_testname})
endmacro (VleBuildTestTemp)

VleBuildTestTemp(test_file_reader test_table_file_reader.cpp)
VleBuildTestTemp(test_vle_results_text_reader test_vle_results_text_reader.cpp)
VleBuildTestTemp(test_tester test_tester.cpp)


