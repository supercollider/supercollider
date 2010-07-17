set(tests
  ampmod_test.cpp
  simd_binary_tests.cpp
  simd_math_tests.cpp
  simd_memory_tests.cpp
  simd_mix_tests.cpp
  simd_pan_tests.cpp
  simd_peak_tests.cpp
  simd_round_tests.cpp
  simd_ternary_tests.cpp
  simd_tests.cpp
  simd_unary_tests.cpp
  simd_unit_conversion_tests.cpp
  softclip_test.cpp
  vec_test.cpp
)

# build benchmarks
foreach(test ${tests})
  string(REPLACE .cpp "" test_name ${test} )
  add_executable(${test_name} ${test})
  add_test(${test_name}_run ${EXECUTABLE_OUTPUT_PATH}/${test_name})
endforeach(test)
