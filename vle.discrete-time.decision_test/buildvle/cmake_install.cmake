# Install script for directory: /home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/pchabrier/.vle/pkgs-2.0/vle.discrete-time.decision_test")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/Authors.txt"
    "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/Description.txt"
    "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/License.txt"
    "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/News.txt"
    "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/Readme.txt"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/data/cmake_install.cmake")
  include("/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/metadata/cmake_install.cmake")
  include("/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/exp/cmake_install.cmake")
  include("/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/src/cmake_install.cmake")
  include("/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/pchabrier/DEVS/packages-2.0/vle.discrete-time.decision_test/buildvle/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
