include_directories(${CMAKE_SOURCE_DIR}/src 
  ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} 
  ${Boost_LIBRARY_DIRS}
  ${CMAKE_SOURCE_DIR}/lib)

ADD_EXECUTABLE(test_Euler test_Euler.cpp)
TARGET_LINK_LIBRARIES(test_Euler
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_Euler test_Euler)

ADD_EXECUTABLE(test_Euler_perturbation test_Euler_perturbation.cpp)
TARGET_LINK_LIBRARIES(test_Euler_perturbation
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_Euler_perturbation test_Euler_perturbation)

ADD_EXECUTABLE(test_RK4 test_RK4.cpp)
TARGET_LINK_LIBRARIES(test_RK4
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_RK4 test_RK4)

ADD_EXECUTABLE(test_QSS2 test_QSS2.cpp)
TARGET_LINK_LIBRARIES(test_QSS2
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_QSS2 test_QSS2)

ADD_EXECUTABLE(test_QSS2_perturbation test_QSS2_perturbation.cpp)
TARGET_LINK_LIBRARIES(test_QSS2_perturbation
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_QSS2_perturbation test_QSS2_perturbation)

ADD_EXECUTABLE(test_coupling test_coupling.cpp)
TARGET_LINK_LIBRARIES(test_coupling
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_coupling test_coupling)

ADD_EXECUTABLE(test_corrections test_corrections.cpp)
TARGET_LINK_LIBRARIES(test_corrections
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY})
ADD_TEST(test_corrections test_corrections)
