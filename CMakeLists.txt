cmake_minimum_required(VERSION 3.5)

project(range_csv LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

find_package(BZip2 REQUIRED)

# NOTE: Don't forget to install
# sudo apt install libbz2-dev

add_library(range_csv range_csv.cpp range_csv.h)
target_link_libraries(range_csv PUBLIC ${BZIP2_LIBRARIES})
target_include_directories(range_csv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
