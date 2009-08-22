echo create zip file...

if $%1 == $ goto error

rmdir /s \tmp\%1 2>nul
pushd .
mkdir \tmp\%1
cd \tmp\%1
md boost\integer
md libs\integer\doc
md libs\integer\example
md libs\integer\test
popd
copy ..\..\boost\integer\endian.hpp \tmp\%1\boost\integer
copy ..\..\boost\integer\cover_operators.hpp \tmp\%1\boost\integer
copy ..\..\libs\integer\doc\endian.html \tmp\%1\libs\integer\doc
copy ..\..\libs\integer\example\endian_example.cpp \tmp\%1\libs\integer\example
copy ..\..\libs\integer\test\endian_test.cpp \tmp\%1\libs\integer\test
copy ..\..\libs\integer\test\Jamfile.* \tmp\%1\libs\integer\test

pushd \tmp
zip -r %1.zip %1
popd
move \tmp\%1.zip .

goto done

:error
echo usage: zip-endian version
echo   version will be used for both the .zip name and the highest level directory name
echo example: zip-endian endian-1.0

:done