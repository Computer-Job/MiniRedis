cmake_minimum_required(VERSION 4.2.3)
project(MiniRedis VERSION 0.0.1 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(server
    src/server.cpp
)

add_executable(client
    src/client.cpp
    src/lexer.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib
)

set_property(TARGET server PROPERTY CXX_STANDARD 17)
set_property(TARGET client PROPERTY CXX_STANDARD 17)
