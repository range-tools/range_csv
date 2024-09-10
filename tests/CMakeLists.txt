cmake_minimum_required(VERSION 3.6)

project(range_csv_tests LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g")

#add_executable(find_scores_for_member find_scores_for_member.cpp)
add_executable(parse_and_print parse_and_print.cpp)
target_link_libraries(parse_and_print PRIVATE range_csv)
#target_link_libraries(parse_and_print ${BZIP2_LIBRARIES})

#target_link_libraries(parse_and_print PRIVATE range_yaml)

file(GLOB TEST_DATA "*.csv")
file(COPY ${TEST_DATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB TEST_DATA "*.csv.bz2")
file(COPY ${TEST_DATA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
