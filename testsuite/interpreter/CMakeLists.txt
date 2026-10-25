set(test_sources
    gc_array.cpp
    slot.cpp
)

# These are tests that will fail to compile.
set(negative_tests
    pyrslot_int64.cpp
    pyrslot_int8.cpp
    pyrslot_ptr.cpp
    pyrslot_enum.cpp
)

foreach(test ${test_sources})
    string(REPLACE .cpp "" test_name ${test} )
    add_executable(${test_name} ${test})

    target_link_libraries(${test_name} boost_test libsclang)

    target_include_directories(${test_name} PUBLIC
        ${boost_include_dirs}
        ${CMAKE_SOURCE_DIR}/include/common
        ${CMAKE_SOURCE_DIR}/include/lang
        ${CMAKE_SOURCE_DIR}/include/plugin_interface
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/common

        ${CMAKE_SOURCE_DIR}/external_libraries/boost_sync/include

        ${CMAKE_SOURCE_DIR}/lang
        ${CMAKE_SOURCE_DIR}/lang/LangSource
        ${CMAKE_SOURCE_DIR}/lang/LangPrimSource
        ${CMAKE_SOURCE_DIR}/lang/Bison

        ${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
    )

    add_test(interpreter__${test_name} ${test_name})
    set_property(TARGET ${test_name} PROPERTY FOLDER Test/Interpreter)
endforeach(test)

# gc_array must compile the real class library. Mirror the installed, Qt-less
# class library layout when the corresponding features are disabled.
target_compile_definitions(gc_array PRIVATE
    "SC_CLASS_LIB_DIR=\"${CMAKE_SOURCE_DIR}/SCClassLibrary\""
)
if(NOT SC_QT)
    target_compile_definitions(gc_array PRIVATE SC_CLASS_LIB_EXCLUDE_GUI=1)
endif()
if(NOT SC_ABLETON_LINK)
    target_compile_definitions(gc_array PRIVATE SC_CLASS_LIB_EXCLUDE_ABLETON=1)
endif()





foreach(test ${negative_tests})
    string(REPLACE .cpp "" test_name ${test} )
    add_executable(${test_name} ${test})

    # Don't build these normally
    set_target_properties(${test_name} PROPERTIES
        EXCLUDE_FROM_ALL TRUE
        EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    target_link_libraries(${test_name} boost_test libsclang)

    target_include_directories(${test_name} PUBLIC
        ${boost_include_dirs}
        ${CMAKE_SOURCE_DIR}/include/common
        ${CMAKE_SOURCE_DIR}/include/lang
        ${CMAKE_SOURCE_DIR}/include/plugin_interface
        ${CMAKE_SOURCE_DIR}/include/server
        ${CMAKE_SOURCE_DIR}/common

        ${CMAKE_SOURCE_DIR}/lang
        ${CMAKE_SOURCE_DIR}/lang/LangSource
        ${CMAKE_SOURCE_DIR}/lang/LangPrimSource
    )

    add_test(NAME failing_${test_name}
        COMMAND ${CMAKE_COMMAND} --build . --target ${test_name} --config $<CONFIGURATION>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    set_tests_properties(failing_${test_name} PROPERTIES WILL_FAIL TRUE)
    set_property(TARGET ${test_name} PROPERTY FOLDER Test/Interpreter)
endforeach(test)
