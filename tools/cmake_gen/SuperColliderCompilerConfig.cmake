# Brian Heim
# 2018-08-26
#
# Compiler configuration help for server plugins

# assume we are not mixing C and C++ compiler vendors
cmake_host_system_information(RESULT has_sse QUERY HAS_SSE)
cmake_host_system_information(RESULT has_sse2 QUERY HAS_SSE2)
cmake_host_system_information(RESULT has_sse_fp QUERY HAS_SSE_FP)

# TODO
# if(SC_USE_EXTENDED_SIMD)
#     cmake_host_system_information(RESULT has_sse_mmx QUERY HAS_SSE_MMX)
#     cmake_host_system_information(RESULT has_mmx QUERY HAS_MMX)
#     cmake_host_system_information(RESULT has_mmx_plus QUERY HAS_MMX_PLUS)
# endif()

function(sc_config_compiler_flags target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
        target_compile_options(${target} PUBLIC
            $<$<BOOL:${has_sse}>:-msse>
            $<$<BOOL:${has_sse2}>:-msse2>
            $<$<BOOL:${has_sse_fp}>:-mfpmath=sse>
            $<$<BOOL:${NATIVE}>:-march=native>
            $<$<BOOL:${STRICT}>:-Wall -Wextra -Werror -Wpedantic>
            )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        # these options only apply if we're doing a 32-bit build, otherwise they cause a diagnostic
        # https://stackoverflow.com/questions/1067630/sse2-option-in-visual-c-x64
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            target_compile_options(${target} PUBLIC
                $<$<BOOL:${has_sse}>:/arch:SSE>
                $<$<BOOL:${has_sse2}>:/arch:SSE2>
                )
        endif()
        if(NATIVE)
            message(WARNING "-DNATIVE is not supported with MSVC")
        endif()
        target_compile_options(${target} PUBLIC
            $<$<BOOL:${STRICT}>:"/Wall /WX">
            )
    else()
        message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. You may want to modify SuperColliderCompilerConfig.cmake to add checks for SIMD flags and other optimizations.")
    endif()
endfunction()
