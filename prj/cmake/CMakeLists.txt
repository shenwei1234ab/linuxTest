CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(linuxTest)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../src/)

#源文件目录
#多进程信号量
FILE(GLOB_RECURSE semProducerConsumer_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/multiprocess/ProducerConsumer.cpp)

#linuxTest
FILE(GLOB_RECURSE linux_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src/LinuxTest/src/main.cpp)




#编译选项(输出动态库)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -gdwarf-2 -pg  -fPIC")
#生成动态库


#添加头文件目录
#INCLUDE_DIRECTORIES(../../src/)
#编译选项(动态载入动态库,libevent)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -gdwarf-2 -pg  -rdynamic -ldl -lpthread -levent -lcrypt -std=c++11")

ADD_EXECUTABLE(semProducerConsumer.exe ${semProducerConsumer_SRC})
ADD_EXECUTABLE(linuxTest.exe ${linux_SRC})
#生成可执行文件
#TARGET_LINK_LIBRARIES(spider)

