add_library(sc_consteval STATIC consteval.cpp)
target_include_directories(sc_consteval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(sc_consteval PUBLIC sc_sema sc_parser sc_source sc_diagnostic Boost::thread)
set_property(TARGET sc_consteval PROPERTY FOLDER Libs)

add_executable(sc_consteval_cli main.cpp)
set_target_properties(sc_consteval_cli PROPERTIES OUTPUT_NAME sc_consteval)
target_link_libraries(sc_consteval_cli PRIVATE sc_consteval)
set_property(TARGET sc_consteval_cli PROPERTY FOLDER LangUtils)
