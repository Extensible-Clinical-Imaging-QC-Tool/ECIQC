# Install script for directory: /home/sabsr3/ECIQC/dcmtk-3.6.1_20170228

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xincludex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/dcmtk/config" TYPE FILE FILES
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/config/include/dcmtk/config/osconfig.h"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/config/include/dcmtk/config/arith.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdocx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/dcmtk" TYPE FILE FILES
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/ANNOUNCE"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CHANGES"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/COPYRIGHT"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CREDITS"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/FAQ"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/HISTORY"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/VERSION"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdocx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/dcmtk" TYPE DIRECTORY FILES "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/docs/" FILES_MATCHING REGEX "/CHANGES\\.[^/][^/][^/]$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xcmakex" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk/DCMTKTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk/DCMTKTargets.cmake"
         "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CMakeFiles/Export/lib/cmake/dcmtk/DCMTKTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk/DCMTKTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk/DCMTKTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk" TYPE FILE FILES "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CMakeFiles/Export/lib/cmake/dcmtk/DCMTKTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk" TYPE FILE FILES "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CMakeFiles/Export/lib/cmake/dcmtk/DCMTKTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xcmakex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/dcmtk" TYPE FILE FILES
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/CMakeFiles/DCMTKConfig.cmake"
    "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/DCMTKConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/config/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/doxygen/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/ofstd/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/oflog/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmdata/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmimgle/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmimage/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmjpeg/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmjpls/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmtls/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmnet/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmsr/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmsign/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmwlm/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmqrdb/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmpstat/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmrt/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmiod/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmfg/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmseg/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmtract/cmake_install.cmake")
  include("/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/dcmpmap/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/sabsr3/ECIQC/dcmtk-3.6.1_20170228/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
