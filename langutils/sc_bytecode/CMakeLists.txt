# The VM's opcodes and the buffer bytecode is compiled into, shared by the old compiler in lang/ and the new backend.
add_library(sc_bytecode STATIC
	src/ByteCodeArray.cpp
)

target_include_directories(sc_bytecode PUBLIC include)
target_link_libraries(sc_bytecode PUBLIC sc_source sc_util)
set_property(TARGET sc_bytecode PROPERTY FOLDER Libs)
