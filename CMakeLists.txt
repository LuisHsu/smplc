cmake_minimum_required(VERSION 3.12)
project(smplc)

set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(lib)
add_subdirectory(exec)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    add_subdirectory(test/unit)
    add_subdirectory(test/regression)
endif()