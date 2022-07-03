# CMakeList.txt: MqttUplink 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.5)
option(OPTION  "arm or linux or linux-arm")

#project name 

message(STATUS "compile start ${OPTION}")
message(${CMAKE_CURRENT_SOURCE_DIR})


  # set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
  # set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
  # set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib-standard) 
  # link_directories(${LIBRARY_OUTPUT_PATH})

#库文件路径
#编译添加debug选项
#SET(CMAKE_BUILD_TYPE "Debug") 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11 -lrt ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -std=c++11 -lrt ")
project(test)
#添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#添加cmake子目录

#add_subdirectory(carrier)

#添加动态库的搜索路径
#aux_source_directory(${LIBRARY_OUTPUT_PATH} ./lib-standard)
#aux_source_directory(lib-standard DIR_SRCS)

add_executable(main main.cpp msgQueue.cpp mqtt.cpp cJSON.c)
#连接动态库

target_link_libraries(main mosquitto)

target_link_libraries(main stdc++)
target_link_libraries(main pthread)







