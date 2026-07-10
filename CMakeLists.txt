cmake_minimum_required(VERSION 4.2.3)
project(MiniRed VERSION 0.0.1 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(EchoServer
    src/server.cpp
)

add_executable(EchoClient
    src/client.cpp
)

target_include_directories(EchoServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib
)

target_include_directories(EchoClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib
)

set_property(TARGET EchoServer PROPERTY CXX_STANDARD 17)
set_property(TARGET EchoClient PROPERTY CXX_STANDARD 17)
