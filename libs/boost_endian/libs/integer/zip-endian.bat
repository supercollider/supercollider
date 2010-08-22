echo create zip file...

if $%1 == $ goto error

rmdir /s \temp\%1 2>nul
pushd .
mkdir \temp\%1
cd \temp\%1
md doc\html
md boost\integer
md libs\integer\doc
md libs\integer\example
md libs\integer\test
popd
copy ..\..\boost.png \temp\%1
copy ..\..\boost\doc\html\minimal.css \temp\%1\doc\html
copy ..\..\boost\binary_stream.hpp \temp\%1\boost
copy ..\..\boost\integer\endian.hpp \temp\%1\boost\integer
copy ..\..\boost\integer\endian_binary_stream.hpp \temp\%1\boost\integer
copy ..\..\boost\integer\cover_operators.hpp \temp\%1\boost\integer
copy ..\..\libs\integer\doc\endian.html \temp\%1\libs\integer\doc
copy ..\..\libs\integer\example\endian_example.cpp \temp\%1\libs\integer\example
copy ..\..\libs\integer\example\endian_hello_world.cpp \temp\%1\libs\integer\example
copy ..\..\libs\integer\test\endian_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\binary_stream_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\endian_binary_stream_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\endian_in_union_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\endian_operations_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\scoped_enum_emulation_test.cpp \temp\%1\libs\integer\test
copy ..\..\libs\integer\test\Jamfile.* \temp\%1\libs\integer\test

pushd \temp
zip -r %1.zip %1
popd
move \temp\%1.zip .

goto done

:error
echo usage: zip-endian version
echo   version will be used for both the .zip name and the highest level directory name
echo example: zip-endian endian-1.0

:done