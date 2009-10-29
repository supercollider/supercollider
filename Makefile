all:
	scons debug=1 -j 4 server plugins run_tests

run_tests:
	scons debug=1 -j 4 run_tests

server:
	scons server -j 4

debug:
	scons debug_server debug_plugins run_tests -j 4

release:
	scons release_server release_plugins -j 4

benchmarks:
	scons benchmarks -j 4

benchmarks:
	scons benchmarks -j 4

clean:
	scons -c
