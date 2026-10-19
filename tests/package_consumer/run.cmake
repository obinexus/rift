# Installs the RIFT build tree into a scratch prefix, then configures, builds
# and runs tests/package_consumer against it with find_package(RIFT).
#
# Required: RIFT_BUILD_DIR, CONSUMER_SOURCE_DIR, WORK_DIR, GENERATOR.
# Optional: CONFIG, C_COMPILER.

foreach(required RIFT_BUILD_DIR CONSUMER_SOURCE_DIR WORK_DIR GENERATOR)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "run.cmake: ${required} is not set")
    endif()
endforeach()

set(prefix "${WORK_DIR}/install")
set(consumer_build "${WORK_DIR}/build")
file(REMOVE_RECURSE "${WORK_DIR}")

set(config_args)
set(build_type_arg)
if(CONFIG)
    set(config_args --config "${CONFIG}")
    set(build_type_arg "-DCMAKE_BUILD_TYPE=${CONFIG}")
endif()
set(compiler_arg)
if(C_COMPILER)
    set(compiler_arg "-DCMAKE_C_COMPILER=${C_COMPILER}")
endif()

function(run_step description)
    execute_process(COMMAND ${ARGN}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE output)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${description} failed (${result}):\n${output}")
    endif()
endfunction()

run_step("install" "${CMAKE_COMMAND}" --install "${RIFT_BUILD_DIR}"
    --prefix "${prefix}" ${config_args})
run_step("consumer configure" "${CMAKE_COMMAND}"
    -S "${CONSUMER_SOURCE_DIR}" -B "${consumer_build}"
    -G "${GENERATOR}" "-DCMAKE_PREFIX_PATH=${prefix}"
    ${compiler_arg} ${build_type_arg})
run_step("consumer build" "${CMAKE_COMMAND}" --build "${consumer_build}"
    ${config_args})

file(GLOB_RECURSE consumer_exe
    "${consumer_build}/rift_package_consumer"
    "${consumer_build}/rift_package_consumer.exe")
list(LENGTH consumer_exe consumer_count)
if(NOT consumer_count EQUAL 1)
    message(FATAL_ERROR "expected one rift_package_consumer binary, found: ${consumer_exe}")
endif()
run_step("consumer run" "${consumer_exe}")
message(STATUS "package consumer: PASS")
