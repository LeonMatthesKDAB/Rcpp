project(test-weak VERSION 0.1 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    tst_weak.cpp
)

target_link_libraries(${PROJECT_NAME} Rcpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
