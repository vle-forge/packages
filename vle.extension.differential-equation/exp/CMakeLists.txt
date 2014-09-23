install(FILES LadybirdPlantlouse.vpz LadybirdPlantlouseXY.vpz 
 PerturbLadybirdPlantlouse.vpz PerturbLadybirdPlantlouseXY.vpz
 LotkaVolterra.vpz LotkaVolterraXY.vpz LotkaVolterraOutputPeriod.vpz
 PerturbLotkaVolterra.vpz PerturbLotkaVolterraXY.vpz 
 Seir.vpz SeirXY.vpz PerturbSeirXY.vpz ExtUpLV.vpz
 DESTINATION exp)

install(FILES profiling/LotkaVolterra.vpz
 DESTINATION exp/profiling)