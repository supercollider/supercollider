add_executable(sc_explorer main.cpp)
target_link_libraries(sc_explorer PRIVATE sc_describe sc_compiler)
set_property(TARGET sc_explorer PROPERTY FOLDER LangUtils)
