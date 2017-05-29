CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(vblib)
SET(VBLIB_ROOT ../../../vnlib2/prj/cmake)
#INCLUDE(../../../vnlib2/prj/cmake/common.cmake)
INCLUDE_DIRECTORIES(${VBLIB_ROOT}/../../inc/base)
INCLUDE_DIRECTORIES(${VBLIB_ROOT}/../../inc/lex)
INCLUDE_DIRECTORIES(${VBLIB_ROOT}/../../inc/net)
INCLUDE_DIRECTORIES(${VBLIB_ROOT}/../../inc/net_ssl)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DDEBUG)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
ELSE()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libgcc -static-libstdc++ -static -pthread -Wall -Werror ")


#INCLUDE(../../../vnlib2/prj/cmake/base.cmake)
#INCLUDE(../../../vnlib2/prj/cmake/lex.cmake)
#INCLUDE(../../../vnlib2/prj/cmake/net.cmake)
#INCLUDE(../../../vnlib2/prj/cmake/netssl.cmake)
#INCLUDE(../../../vnlib2/prj/cmake/pktcoder.cmake)

FILE(GLOB VN_BASE_SRC ${VBLIB_ROOT}/../../src/base/*.cpp)
#ADD_LIBRARY(vnbase SHARED ${VN_BASE_SRC})
ADD_LIBRARY(vnbase STATIC ${VN_BASE_SRC})

FILE(GLOB VN_LEX_SRC ${VBLIB_ROOT}/../../src/lex/*.cpp)
ADD_LIBRARY(vnlex STATIC ${VN_LEX_SRC})

FILE(GLOB VN_NET_SRC ${VBLIB_ROOT}/../../src/net/*.cpp)
ADD_LIBRARY(vnnet STATIC ${VN_NET_SRC})	

FILE(GLOB VN_NETSSL_SRC ${VBLIB_ROOT}/../../src/net_ssl/*.cpp)
ADD_LIBRARY(vnnetssl STATIC ${VN_NETSSL_SRC})
