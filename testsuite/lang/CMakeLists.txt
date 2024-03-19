add_library(lang_boost_test STATIC boost_test.cpp)
target_include_directories(lang_boost_test PUBLIC ${boost_include_dirs})

add_subdirectory(LangSource)

file(GLOB tests *.scd)
foreach(test ${tests})
    get_filename_component(test_name ${test} NAME_WE)
    set(test_target ${test_name}_run)
    add_test(NAME ${test_target} COMMAND sclang -l ${CMAKE_BINARY_DIR}/build_sclang.cfg ${test})
endforeach()
