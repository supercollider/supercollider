message(STATUS "Running deploy script")
execute_process(COMMAND "${CMAKE_SOURCE_DIR}/vs2013/deploy_staging.bat" "${SC_WIN64_INSTALL_STAGING}" "${SC_WIN64_INSTALL_PREFIX}" "$ENV{QT_HOME}")
