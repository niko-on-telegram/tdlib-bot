cmake_minimum_required(VERSION 3.26)
project(tdlib_bot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.12.0")
# version 1.8.19
CPMAddPackage("gh:tdlib/td#2589c3f")

add_executable(tdlib_bot main.cpp)
target_link_libraries(tdlib_bot PUBLIC Td::TdStatic spdlog)

#tdlib requires C++14, but we actually need it only for these targets
set_target_properties(tdutils tdcore PROPERTIES CXX_STANDARD 14)