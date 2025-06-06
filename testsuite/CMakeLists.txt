add_subdirectory(server)
add_subdirectory(sclang)

add_custom_target(sclang_lang_testsuite
	COMMAND
		# use CMAKE_COMMAND to wrap the command to be executed w/ env variables
		${CMAKE_COMMAND} -E env
		$<TARGET_FILE:sclang>
		"--exclude-default-paths"
		"--include-path"
			"${CMAKE_SOURCE_DIR}/SCClassLibrary"
		"--include-path"
			"${CMAKE_SOURCE_DIR}/testsuite/classlibrary"
		"${CMAKE_SOURCE_DIR}/testsuite/scripts/sclang_test_runner.scd"
			"${CMAKE_SOURCE_DIR}/testsuite/scripts/test_run_proto_sclang_only.scd"
			"${CMAKE_SOURCE_DIR}/testsuite/scripts/run/sclang_lang_testsuite_result.scxtar"
	DEPENDS
		sclang
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(sclang_full_testsuite
	COMMAND
		${CMAKE_COMMAND} -E env
		SCSYNTH_PATH=$<TARGET_FILE:scsynth>
		SC_UGEN_PLUGINS_PATH=${CMAKE_BINARY_DIR}/server/plugins
		$<TARGET_FILE:sclang>
		"--exclude-default-paths"
		"--include-path"
			"${CMAKE_SOURCE_DIR}/SCClassLibrary"
		"--include-path"
			"${CMAKE_SOURCE_DIR}/testsuite/classlibrary"
		"${CMAKE_SOURCE_DIR}/testsuite/scripts/sclang_test_runner.scd"
			"${CMAKE_SOURCE_DIR}/testsuite/scripts/test_run_proto_all.scd"
			"${CMAKE_SOURCE_DIR}/testsuite/scripts/run/sclang_full_testsuite_result.scxtar"
	DEPENDS
		sclang
		scsynth
		plugins
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# we need plugins to run the testsuite so we need to add them as a dependency to our full testsuite
add_custom_target(plugins
	DEPENDS
		BinaryOpUGens
		ChaosUGens
		DelayUGens
		DemandUGens
		DemoUGens
		DynNoiseUGens
		FilterUGens
		GendynUGens
		GrainUGens
		IOUGens
		LFUGens
		MulAddUGens
		NoiseUGens
		OscUGens
		PanUGens
		PhysicalModelingUGens
		ReverbUGens
		TestUGens
		TriggerUGens
		UnaryOpUGens
		UnpackFFTUGens
		FFT_UGens
		PV_ThirdParty
		ML_UGens
		DiskIO_UGens
)
