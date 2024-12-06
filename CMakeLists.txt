cmake_minimum_required(VERSION 3.18)

include(${dep_cmake_SOURCE_DIR}/VERSION.cmake)

project(
  BGMRPCClient
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

include(${dep_cmake_SOURCE_DIR}/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core WebSockets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core WebSockets
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# include_directories(${BGMRPCCommon_includeDir})

add_library(BGMRPCClient SHARED "")
target_sources(
  BGMRPCClient
  PRIVATE BGMRPCClient_global.h bgmrpcclient.cpp calling.h calling.cpp
  PUBLIC bgmrpcclient.h)

set_target_properties(
  BGMRPCClient
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME BGMRPCClient2
             PUBLIC_HEADER "bgmrpcclient.h"
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_CLIENT}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(BGMRPCClient PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                           Qt${QT_VERSION_MAJOR}::WebSockets)

install(TARGETS BGMRPCClient
        LIBRARY DESTINATION ${BGMRPC_CLIENT}/${CMAKE_INSTALL_LIBDIR}
                COMPONENT BGMRPC_client)
install(
  TARGETS BGMRPCClient
  LIBRARY DESTINATION ${BGMRPC_CLIENT}/${CMAKE_INSTALL_LIBDIR}
          COMPONENT BGMRPC_client_dev
          EXCLUDE_FROM_ALL
  PUBLIC_HEADER
    DESTINATION ${BGMRPC_CLIENT}/${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT BGMRPC_client_dev
    EXCLUDE_FROM_ALL)
