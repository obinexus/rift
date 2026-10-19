# Runs one command and checks its exit status and output.
#
# Required:
#   COMMAND               The command and its arguments, separated by "|".
#   EXPECT_EXIT           The exact exit status expected.
# Optional:
#   WORKING_DIRECTORY     Directory to run the command in.
#   EXPECT_STDOUT_FILE    File whose contents stdout must equal exactly.
#   EXPECT_STDERR_FILE    File whose contents stderr must equal exactly.
#   EXPECT_STDOUT_REGEX   Regular expression stdout must match.
#   EXPECT_STDERR_REGEX   Regular expression stderr must match.
#   EXPECT_FILE           A file the command must have created (non-empty).
#
# Carriage returns are removed before comparing, so golden files are the same
# on every platform.

if(NOT DEFINED COMMAND OR NOT DEFINED EXPECT_EXIT)
    message(FATAL_ERROR "run_command.cmake: COMMAND and EXPECT_EXIT are required")
endif()
string(REPLACE "|" ";" command "${COMMAND}")
if(NOT WORKING_DIRECTORY)
    set(WORKING_DIRECTORY ".")
endif()
if(EXPECT_FILE)
    file(REMOVE "${EXPECT_FILE}")
endif()

execute_process(COMMAND ${command}
    WORKING_DIRECTORY "${WORKING_DIRECTORY}"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr)
string(REPLACE "\r" "" stdout "${stdout}")
string(REPLACE "\r" "" stderr "${stderr}")

set(failures)
if(NOT status STREQUAL "${EXPECT_EXIT}")
    list(APPEND failures "exit status ${status}, expected ${EXPECT_EXIT}")
endif()
foreach(stream stdout stderr)
    string(TOUPPER "${stream}" upper)
    if(EXPECT_${upper}_FILE)
        file(READ "${EXPECT_${upper}_FILE}" golden)
        string(REPLACE "\r" "" golden "${golden}")
        if(NOT "${${stream}}" STREQUAL "${golden}")
            list(APPEND failures
                "${stream} differs from ${EXPECT_${upper}_FILE}\n--- expected\n${golden}--- actual\n${${stream}}---")
        endif()
    endif()
    if(DEFINED EXPECT_${upper}_REGEX AND NOT "${${stream}}" MATCHES "${EXPECT_${upper}_REGEX}")
        list(APPEND failures "${stream} does not match '${EXPECT_${upper}_REGEX}'")
    endif()
endforeach()
if(EXPECT_FILE)
    if(NOT EXISTS "${EXPECT_FILE}")
        list(APPEND failures "${EXPECT_FILE} was not created")
    else()
        file(SIZE "${EXPECT_FILE}" size)
        if(size EQUAL 0)
            list(APPEND failures "${EXPECT_FILE} is empty")
        endif()
    endif()
endif()

if(failures)
    string(REPLACE ";" "\n" failures "${failures}")
    message(FATAL_ERROR "${failures}\n\nstdout:\n${stdout}\nstderr:\n${stderr}")
endif()
