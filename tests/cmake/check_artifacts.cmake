# Checks the Linux artifact layout that .github/workflows/build.yml relies on:
# build/bin/linux-<gcc|clang>/{rift,riftlang,rifttest}.{a,so}, with the public
# entry points exported from each shared library.
#
# The expected directory is derived here from the compiler ID, independently
# of RIFT_PLATFORM_TAG, and compared with where CMake actually put the targets.
#
# Required: SOURCE_DIR, COMPILER_ID, NM, and for each component
# <COMPONENT>_STATIC and <COMPONENT>_SHARED (the target file paths).
# Optional: CONFIG.

if(COMPILER_ID STREQUAL "GNU")
    set(compiler "gcc")
elseif(COMPILER_ID STREQUAL "Clang")
    set(compiler "clang")
else()
    message(FATAL_ERROR "check_artifacts.cmake: unsupported compiler ID '${COMPILER_ID}'")
endif()

set(expected_dir "${SOURCE_DIR}/build/bin/linux-${compiler}")
if(CONFIG STREQUAL "Debug")
    string(APPEND expected_dir "/debug")
endif()

set(failures 0)
foreach(component RIFT RIFTLANG RIFTTEST)
    string(TOLOWER "${component}" name)
    foreach(kind STATIC SHARED)
        if(kind STREQUAL "STATIC")
            set(expected "${expected_dir}/${name}.a")
        else()
            set(expected "${expected_dir}/${name}.so")
        endif()
        set(actual "${${component}_${kind}}")
        if(NOT actual STREQUAL expected)
            message(SEND_ERROR "${name} (${kind}) was built at\n  ${actual}\nbut CI expects\n  ${expected}")
            math(EXPR failures "${failures} + 1")
        elseif(NOT EXISTS "${actual}")
            message(SEND_ERROR "missing artifact: ${actual}")
            math(EXPR failures "${failures} + 1")
        endif()
    endforeach()
endforeach()

if(failures EQUAL 0)
    foreach(pair "RIFT;rift_regex_compile" "RIFTLANG;rlang_compile" "RIFTTEST;rtest_run_suite")
        list(GET pair 0 component)
        list(GET pair 1 symbol)
        execute_process(COMMAND "${NM}" -D "${${component}_SHARED}"
            RESULT_VARIABLE result OUTPUT_VARIABLE symbols ERROR_VARIABLE errors)
        if(NOT result EQUAL 0 OR NOT symbols MATCHES " T ${symbol}\n")
            message(SEND_ERROR "${${component}_SHARED} does not export ${symbol} ${errors}")
            math(EXPR failures "${failures} + 1")
        endif()
    endforeach()
endif()

if(NOT failures EQUAL 0)
    message(FATAL_ERROR "${failures} artifact check(s) failed")
endif()
message(STATUS "artifacts in ${expected_dir}: PASS")
