add_custom_target(
	create_error_regression_file
	COMMAND $<TARGET_FILE:sclang> 
		-a 
		--include-path SCClassLibrary/ 
		--include-path testsuite/sclangErrorRegressions 
		testsuite/sclangErrorRegressions/generateError.scd 
		testsuite/sclangErrorRegressions/regression.txt
	DEPENDS sclang
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Creates the sclang error regression file, used so we can monitor how error reporting progresses in the class library."
)

set_property(TARGET create_error_regression_file PROPERTY FOLDER Util)
