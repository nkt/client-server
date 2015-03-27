cmake_minimum_required(VERSION 3.1)
project(client_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(CLIENT_SOURCE_FILES client.c)
add_executable(client ${CLIENT_SOURCE_FILES})

set(SERVER_SOURCE_FILES server.c)
add_executable(server ${SERVER_SOURCE_FILES})
