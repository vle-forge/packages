INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(${VLE_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(glue SHARED Average.cpp Dispatch.cpp Product.cpp Sum.cpp
		 	Switch.cpp WeightedProduct.cpp WeightedSum.cpp
			Constant.cpp Scale.cpp MovingAverage.cpp
			Identity.cpp DifferenceEquationAdaptor.cpp)

TARGET_LINK_LIBRARIES(glue ${VLE_LIBRARIES} ${Boost_DATE_TIME_LIBRARY})

INSTALL(TARGETS glue
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_SUBDIRECTORY(test)
