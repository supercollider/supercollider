all:
	scons debug=1 -j 4

run_tests:
	scons debug=1 -j 4 run_tests

server:
	scons server -j 4

debug:
	scons debug_server -j 4

release:
	scons release_server -j 4

clean:
	scons -c
