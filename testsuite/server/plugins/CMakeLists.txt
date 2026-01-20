
add_library(c_api_test MODULE c_api_test.c)

target_include_directories(
    c_api_test
    PRIVATE ${CMAKE_SOURCE_DIR}/include/common
    PRIVATE ${CMAKE_SOURCE_DIR}/include/plugin_interface
)
