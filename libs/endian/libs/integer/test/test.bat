set ENDIAN_LOCATE_ROOT=%temp%\endian-regr
md %ENDIAN_LOCATE_ROOT% 2>nul

echo Begin test processing...
bjam --dump-tests "-sALL_LOCATE_TARGET=%ENDIAN_LOCATE_ROOT%" %* >bjam.log 2>&1
echo Begin log processing...
process_jam_log %ENDIAN_LOCATE_ROOT% <bjam.log
start bjam.log
echo Begin compiler status processing...
compiler_status --locate-root %ENDIAN_LOCATE_ROOT% ..\..\.. test_status.html test_links.html
start test_status.html
