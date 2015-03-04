ADD_LIBRARY(lua STATIC
  lauxlib.c
  lcode.c
  ldebug.c
  ldo.h
  lfunc.h
  linit.c
  llex.h
  lmem.c
  lobject.c
  lopcodes.h
  lparser.h
  lstring.c
  ltable.c
  ltm.c
  luac.c
  lualib.h
  lvm.c
  lzio.h
  lapi.c
  lauxlib.h
  lcode.h
  ldebug.h
  ldump.c
  lgc.c
  liolib.c
  llimits.h
  lmem.h
  lobject.h
  loslib.c
  lstate.c
  lstring.h
  ltable.h
  ltm.h
  luaconf.h
  lundump.c
  lvm.h
  print.c
  lapi.h
  lbaselib.c
  ldblib.c
  ldo.c
  lfunc.c
  lgc.h
  llex.c
  lmathlib.c
  loadlib.c
  lopcodes.c
  lparser.c
  lstate.h
  lstrlib.c
  ltablib.c
  lua.c
  lua.h
  lundump.h
  lzio.c)

IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_COMPILER_IS_GNUCXX)
    SET_TARGET_PROPERTIES(lua PROPERTIES COMPILE_FLAGS -fPIC)
  endif ()
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

SET_TARGET_PROPERTIES(lua PROPERTIES OUTPUT_NAME
  "lua-${MODEL_MAJOR}.${MODEL_MINOR}")

TARGET_LINK_LIBRARIES(lua ${VLE_LIBRARIES})

INSTALL(TARGETS lua ARCHIVE DESTINATION lib)

install(FILES lauxlib.h lua.h lua.hpp luaconf.h lualib.h
  DESTINATION src)
