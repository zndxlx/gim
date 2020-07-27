# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (app)

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libevent/include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/libevent/include)
# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 编译libevent
set(EVENT__DISABLE_TESTS ON)
set(EVENT__DISABLE_OPENSSL ON)
set(EVENT__DISABLE_REGRESS ON)
set(EVENT__DISABLE_BENCHMARK ON)
set(EVENT__DISABLE_SAMPLES ON)
set(EVENT__LIBRARY_TYPE STATIC)

add_subdirectory(libevent libevent)


add_subdirectory(evpp evpp)

add_executable(${PROJECT_NAME} main.cc GConnection.cc message.cc command.cc)


#指定库路径
link_directories(${CMAKE_BINARY_DIR}/libevent/lib)
link_directories(${CMAKE_BINARY_DIR}/evpp/lib)
# 添加链接库
target_link_libraries(${PROJECT_NAME} event_core event_extra evpp_static)
#target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/third/libevent-2.1.12-stable/lib/event.lib)
