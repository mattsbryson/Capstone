cmake_minimum_required (VERSION 2.6)

project (vrx)
set (VRX_VERSION_MAJOR 0)
set (VRX_VERSION_MINOR 1)

option (ENABLE_OPENNI "Create the OpenNI Module" OFF)
set (OPENNI_INCLUDE "/usr/include/ni")

option (ENABLE_HYDRA "Create the Hydra Module" OFF)
set (SIXENSE_PATH "${PROJECT_SOURCE_DIR}/../sixenseSDK_linux_OSX" CACHE PATH "Sixense SDK for Hydra")

# set the default install to be our binary directory
#set (CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/release" CACHE PATH "install prefix" FORCE)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX
    "${PROJECT_BINARY_DIR}/release" CACHE PATH "vrx install prefix" FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

set (EXTRA_LIBS X11 GL Xtst Xdamage)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

include_directories ("${PROJECT_SOURCE_DIR}/vertex")
add_subdirectory (vertex)
set (EXTRA_LIBS ${EXTRA_LIBS} vertex)

if (ENABLE_OPENNI)
  include_directories ("${PROJECT_SOURCE_DIR}/openni")
  add_subdirectory (openni)
  set (EXTRA_LIBS ${EXTRA_LIBS} openni)

  set (EXTRA_LIBS ${EXTRA_LIBS} OpenNI XnVNite_1_5_2)
endif (ENABLE_OPENNI)

if (ENABLE_HYDRA)
  include_directories ("${PROJECT_SOURCE_DIR}/hydra")
  add_subdirectory (hydra)
  set (EXTRA_LIBS ${EXTRA_LIBS} hydra)

  # link in the sixense library
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    # 64 bit
    link_directories ("${SIXENSE_PATH}/lib/linux_x64/release")
    set (EXTRA_LIBS ${EXTRA_LIBS} sixense_x64)
    install (FILES "${SIXENSE_PATH}/lib/linux_x64/release/libsixense_x64.so" DESTINATION bin)
  else (CMAKE_SIZEOF_VOID_P MATCHES "8")
    # 32 bit
    link_directories ("${SIXENSE_PATH}/lib/linux/release")
    set (EXTRA_LIBS ${EXTRA_LIBS} sixense)
    install (FILES "${SIXENSE_PATH}/lib/linux/release/libsixense.so" DESTINATION bin)
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
  # this is needed for some c++ symbols for some reason
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--unresolved-symbols=ignore-in-shared-libs")
endif (ENABLE_HYDRA)

add_executable (vrx vrx.cpp backend.cpp frontend.cpp bridge.cpp panels.cpp debug.cpp)
#add_executable (vrx vrx.cpp frontend.cpp)

target_link_libraries (vrx ${EXTRA_LIBS})

install (TARGETS vrx DESTINATION bin)
install (FILES "${PROJECT_SOURCE_DIR}/run.sh"
               "${PROJECT_SOURCE_DIR}/server0.sh"
               "${PROJECT_SOURCE_DIR}/debug.sh"
               "${PROJECT_SOURCE_DIR}/cdebug.sh"
         DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                          GROUP_READ GROUP_EXECUTE
                          WORLD_READ WORLD_EXECUTE)

add_test (vrxRuns vrx)

