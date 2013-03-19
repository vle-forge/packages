include_directories(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

link_directories(${VLE_LIBRARY_DIRS} ${CMAKE_SOURCE_DIR}/lib)

ADD_LIBRARY(Perturb MODULE Perturb.cpp)
TARGET_LINK_LIBRARIES(Perturb ${VLE_LIBRARIES})
INSTALL(TARGETS Perturb
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

ADD_LIBRARY(LotkaVolterra MODULE LotkaVolterra.cpp)
TARGET_LINK_LIBRARIES(LotkaVolterra ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LotkaVolterra
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

    
ADD_LIBRARY(LotkaVolterraY MODULE LotkaVolterraY.cpp)
TARGET_LINK_LIBRARIES(LotkaVolterraY ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LotkaVolterraY
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
    
ADD_LIBRARY(LotkaVolterraX MODULE LotkaVolterraX.cpp)
TARGET_LINK_LIBRARIES(LotkaVolterraX ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LotkaVolterraX
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

   
ADD_LIBRARY(LadybirdPlantlouse MODULE LadybirdPlantlouse.cpp)
TARGET_LINK_LIBRARIES(LadybirdPlantlouse ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LadybirdPlantlouse
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

ADD_LIBRARY(LadybirdPlantlouseX MODULE LadybirdPlantlouseX.cpp)
TARGET_LINK_LIBRARIES(LadybirdPlantlouseX ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LadybirdPlantlouseX
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
    
ADD_LIBRARY(LadybirdPlantlouseY MODULE LadybirdPlantlouseY.cpp)
TARGET_LINK_LIBRARIES(LadybirdPlantlouseY ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS LadybirdPlantlouseY
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

ADD_LIBRARY(Seir MODULE Seir.cpp)
TARGET_LINK_LIBRARIES(Seir ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS Seir
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
    
    
ADD_LIBRARY(Sm MODULE Sm.cpp)
TARGET_LINK_LIBRARIES(Sm ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS Sm
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
    
ADD_LIBRARY(Em MODULE Em.cpp)
TARGET_LINK_LIBRARIES(Em ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS Em
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)    
        
ADD_LIBRARY(Rm MODULE Rm.cpp)
TARGET_LINK_LIBRARIES(Rm ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS Rm
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)

ADD_LIBRARY(Im MODULE Im.cpp)
TARGET_LINK_LIBRARIES(Im ${VLE_LIBRARIES} differential-equation)
INSTALL(TARGETS Im
    RUNTIME DESTINATION plugins/simulator
    LIBRARY DESTINATION plugins/simulator)
 
