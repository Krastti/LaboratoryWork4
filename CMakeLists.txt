cmake_minimum_required(VERSION 4.0)

project(LaboratoryWork4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(LaboratoryWork4
  main.cpp
  src/state_machine.cpp
)

target_include_directories(LaboratoryWork4
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(LaboratoryWork4Tests
  tests/test.cpp
  src/state_machine.cpp
)

target_include_directories(LaboratoryWork4Tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)