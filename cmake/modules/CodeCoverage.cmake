# CodeCoverage.cmake - CMake module to enable code coverage reporting
#
# Adds targets that generate code coverage reports:
#  setup_target_for_coverage_lcov(NAME <name> EXECUTABLE <exe> DEPENDENCIES <deps>)
#    Generates an HTML coverage report using lcov/genhtml
#
# Based on code by Lars Bilke and others

include(CMakeParseArguments)

# Check prerequisites
find_program(GCOV_PATH gcov)
find_program(LCOV_PATH NAMES lcov)
find_program(GENHTML_PATH NAMES genhtml)

if(NOT GCOV_PATH)
    message(FATAL_ERROR "gcov not found! Aborting...")
endif()

if(NOT LCOV_PATH)
    message(FATAL_ERROR "lcov not found! Aborting...")
endif()

if(NOT GENHTML_PATH)
    message(FATAL_ERROR "genhtml not found! Aborting...")
endif()

# Setup coverage target
function(setup_target_for_coverage_lcov)
    set(options NONE)
    set(oneValueArgs NAME EXECUTABLE)
    set(multiValueArgs DEPENDENCIES)
    cmake_parse_arguments(Coverage "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Setup target
    add_custom_target(${Coverage_NAME}
        # Cleanup lcov
        COMMAND ${LCOV_PATH} --directory . --zerocounters

        # Run tests
        COMMAND ${Coverage_EXECUTABLE}

        # Capture lcov counters and generate report
        COMMAND ${LCOV_PATH} --directory . --capture --output-file ${Coverage_NAME}.info
        COMMAND ${LCOV_PATH} --remove ${Coverage_NAME}.info '/usr/*' '*/test/*' --output-file ${Coverage_NAME}.info.cleaned
        COMMAND ${GENHTML_PATH} -o ${Coverage_NAME} ${Coverage_NAME}.info.cleaned
        COMMAND ${CMAKE_COMMAND} -E remove ${Coverage_NAME}.info ${Coverage_NAME}.info.cleaned

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${Coverage_DEPENDENCIES}
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
    )

    # Show info where to find the report
    add_custom_command(TARGET ${Coverage_NAME} POST_BUILD
        COMMAND ;
        COMMENT "Open ./${Coverage_NAME}/index.html in your browser to view the coverage report."
    )
endfunction()