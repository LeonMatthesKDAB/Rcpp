cmake_minimum_required(VERSION 3.0.0)

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  project(Rcpp
          LANGUAGES CXX
          VERSION 0.95.0
          )
else()
  project(Rcpp
          DESCRIPTION "Better Reference counting smart pointers"
          LANGUAGES CXX
          VERSION 0.95.0
          )
endif()

include(FeatureSummary)

option(${PROJECT_NAME}_TESTS "Build the tests" ON)
option(${PROJECT_NAME}_EXAMPLES "Build the examples" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
  endif()

  message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME}_VERSION} in ${CMAKE_BUILD_TYPE} mode.")
endif()

add_subdirectory(src/rcpp)

if(${PROJECT_NAME}_IS_ROOT_PROJECT)

  if(${PROJECT_NAME}_TESTS)
    enable_testing()
    add_subdirectory(tests)
  endif()
  # if(${PROJECT_NAME}_EXAMPLES)
    # add_subdirectory(examples)
  # endif()

endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
