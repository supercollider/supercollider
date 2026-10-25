add_executable(sc_inspect main.cpp )
target_link_libraries(sc_inspect PRIVATE sc_describe sc_compiler Boost::thread)
set_property(TARGET sc_inspect PROPERTY FOLDER LangUtils)
