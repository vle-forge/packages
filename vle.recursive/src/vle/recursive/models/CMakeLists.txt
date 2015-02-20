INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src
    ${VLE_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(
    ${VLE_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})
ADD_LIBRARY(MetaManagerDyn MODULE "MetaManagerDyn.cpp")
TARGET_LINK_LIBRARIES(MetaManagerDyn
    ${VLE_LIBRARIES}
    ${Boost_LIBRARIES}
    vle-recursive)
INSTALL(TARGETS MetaManagerDyn
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

