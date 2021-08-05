cmake_minimum_required(VERSION 3.0.0)
project(Rcpp-Tests)

include_directories(./doctest)

add_subdirectory(./common)

add_subdirectory(rc)
add_subdirectory(weak)
add_subdirectory(prc)
add_subdirectory(pweak)
