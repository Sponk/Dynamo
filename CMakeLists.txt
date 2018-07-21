cmake_minimum_required(VERSION 2.6)
project(dynamo)

add_executable(dynamo main.cpp)

install(TARGETS dynamo RUNTIME DESTINATION bin)
