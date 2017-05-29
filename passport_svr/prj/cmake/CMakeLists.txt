CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(passport_svr)

INCLUDE(../../../vnlib2/prj/cmake/common.cmake)
INCLUDE(../../../vnlib2/prj/cmake/base.cmake)
INCLUDE(../../../vnlib2/prj/cmake/lex.cmake)
INCLUDE(../../../vnlib2/prj/cmake/net.cmake)
INCLUDE(../../../vnlib2/prj/cmake/netssl.cmake)
INCLUDE(../../../vnlib2/prj/cmake/pktcoder.cmake)

INCLUDE_DIRECTORIES(../../dep/include/mysql)
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../dep/lib/${CMAKE_SYSTEM_NAME})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../src/)

FILE(GLOB_RECURSE PASSPORT_COMMON_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/common/*.cpp)
FILE(GLOB_RECURSE PASSPORT_SERVER_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/passportsvr/*.cpp)
FILE(GLOB PASSPORT_DB_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/db/*.cpp)
FILE(GLOB PASSPORT_CRYPTOGRAPHY_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/cryptography/*.cpp)
FILE(GLOB PASSPORT_CURL_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/curl/*.cpp)
FILE(GLOB PASSPORT_CRYPT_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/crypt/*.cpp)
FILE(GLOB_RECURSE PASSPORT_TOOL_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/tool/*.cpp)
FILE(GLOB_RECURSE PASSPORT_THIRD_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/third/*.cpp)
FILE(GLOB_RECURSE FRAMEWORK_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/framework/*.cpp)
FILE(GLOB_RECURSE TRANBASE_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/tranbase/*.cpp)
FILE(GLOB_RECURSE MANAGER_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/manager/*.cpp)

ADD_EXECUTABLE(spider
	${FRAMEWORK_SRC}
	${TRANBASE_SRC}
	${MANAGER_SRC}      
	${PASSPORT_COMMON_SRC} 
	${PASSPORT_SERVER_SRC}
	${PASSPORT_DB_SRC} 
	${PASSPORT_CRYPTOGRAPHY_SRC} 
	${PASSPORT_CURL_SRC}
	${PASSPORT_CRYPT_SRC} 
	${PASSPORT_TOOL_SRC}
	${PASSPORT_THIRD_SRC}
	)


TARGET_LINK_LIBRARIES(spider vnnetssl vnnet vnlex vnbase  mysqlclient dl rt ssl crypto)

FILE(GLOB CONSOLE_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/console/*.cpp)

ADD_EXECUTABLE(console 
	${FRAMEWORK_SRC}  
	${PASSPORT_COMMON_SRC} 
	${CONSOLE_SRC})
TARGET_LINK_LIBRARIES(console vnnet vnlex vnbase dl rt ssl crypto)
