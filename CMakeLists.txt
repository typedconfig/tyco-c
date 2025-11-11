cmake_minimum_required(VERSION 3.16)
project(tyco-c LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(../tyco-cpp tyco-cpp-build EXCLUDE_FROM_ALL)
set_target_properties(tyco PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(tyco_c SHARED src/tyco_c_api.cpp)
target_include_directories(tyco_c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../tyco-cpp/include)
target_link_libraries(tyco_c PRIVATE tyco)

enable_testing()
add_executable(tyco_c_tests tests/c_api_test.cpp)
target_include_directories(tyco_c_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ../tyco-cpp/include)
target_link_libraries(tyco_c_tests PRIVATE tyco_c nlohmann_json::nlohmann_json)
target_compile_definitions(tyco_c_tests PRIVATE TYCO_TEST_SUITE_DIR="${CMAKE_SOURCE_DIR}/../tyco-test-suite")
add_test(NAME tyco_c_tests COMMAND tyco_c_tests)
