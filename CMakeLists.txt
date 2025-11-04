cmake_minimum_required(VERSION 3.27)
project(aura-os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(GTest REQUIRED)