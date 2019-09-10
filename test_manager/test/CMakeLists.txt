#VleBuildAllTest()

VleBuildTest(test_manager test_manager.cpp)
VleBuildTest(test_manager_threads test_manager_threads.cpp)

##REQUIRE cvle 
VleBuildTest(test_manager_mpi test_manager_mpi.cpp)
