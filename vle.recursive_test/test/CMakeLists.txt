#VleBuildAllTest()

VleBuildTest(test_accumulators test_accumulators.cpp)
VleBuildTest(test_vle_recursive test_vle_recursive.cpp)
VleBuildTest(test_ExBohachevskyMM test_ExBohachevskyMM.cpp)
VleBuildTest(test_LogExMM test_LogExMM.cpp)

##REQUIRE mvle 
VleBuildTest(test_ExBohachevskyMM_mvle test_ExBohachevskyMM_mvle.cpp)

