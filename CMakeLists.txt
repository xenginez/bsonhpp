cmake_minimum_required(VERSION 3.5)

project (bsonhpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

add_executable(bsonhpp sample.cpp)