project(test-common VERSION 0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
  InstanceCounter.cpp
  MemoryGuard.cpp
  )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    )

