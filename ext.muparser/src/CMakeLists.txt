ADD_LIBRARY(muParser STATIC
  muParserBase.h
  muParserError.h
  muParserTemplateMagic.h
  muParserBytecode.h
  muParserFixes.h
  muParserTest.h
  muParserCallback.h
  muParser.h
  muParserToken.h
  muParserDef.h
  muParserInt.h
  muParserTokenReader.h
  muParserDLL.h
  muParserStack.h
  muParserBase.cpp
  muParser.cpp
  muParserInt.cpp
  muParserBytecode.cpp
  muParserDLL.cpp
  muParserTest.cpp
  muParserCallback.cpp
  muParserError.cpp
  muParserTokenReader.cpp)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(muParser PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(muParser PROPERTIES OUTPUT_NAME
  "muParser-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(muParser ${VLE_LIBRARIES})

INSTALL(TARGETS muParser ARCHIVE DESTINATION lib)