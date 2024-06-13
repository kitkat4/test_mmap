cmake_minimum_required(VERSION 3.22)

project(test_mmap)

include_directories(include)

add_executable(write_data
  src/write_data.cpp
)

add_executable(read_data
  src/read_data.cpp
)


