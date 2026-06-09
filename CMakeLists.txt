cmake_minimum_required(VERSION 3.16)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not supported. "
        "Remove CMakeCache.txt and CMakeFiles from the source directory, then "
        "configure with 'cmake -S . -B build/<name>' or run 'make configure'."
    )
endif()

project(RIFT
    VERSION 5.0.0
    DESCRIPTION "RIFT Is a Flexible Translator"
    LANGUAGES C CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

option(RIFT_BUILD_TESTS "Build the RIFT test suite" ON)
option(RIFT_BUILD_EXAMPLES "Build RIFT examples" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(RIFT_PLATFORM_TAG "windows-msvc")
elseif(MINGW)
    set(RIFT_PLATFORM_TAG "windows-mingw")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(RIFT_PLATFORM_TAG "linux-${CMAKE_C_COMPILER_ID}")
else()
    string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_C_COMPILER_ID}" RIFT_PLATFORM_TAG)
endif()
string(TOLOWER "${RIFT_PLATFORM_TAG}" RIFT_PLATFORM_TAG)
set(RIFT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build/bin/${RIFT_PLATFORM_TAG}")

set(CMAKE_REQUIRED_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(RIFT_PUBLIC_HEADER_NAMES
    bridge bytecode cli codec codegen emitter lexer memory parser pattern platform
    regex rift riftbridge riftlang rifttest semantic token version
)
foreach(header_name IN LISTS RIFT_PUBLIC_HEADER_NAMES)
    string(TOUPPER "${header_name}" header_var)
    check_c_source_compiles(
        "#include <rift/${header_name}.h>\nint main(void) { return 0; }"
        "RIFT_HEADER_${header_var}_COMPILES_AS_C"
    )
    if(NOT RIFT_HEADER_${header_var}_COMPILES_AS_C)
        message(FATAL_ERROR "Public header rift/${header_name}.h does not compile independently as C")
    endif()
endforeach()
check_cxx_source_compiles(
    "#include <rift/rift.h>\n#include <rift/riftlang.h>\n#include <rift/rifttest.h>\n#include <rift/riftbridge.h>\nint main() { return 0; }"
    RIFT_CANONICAL_HEADERS_COMPILE_AS_CXX
)
if(NOT RIFT_CANONICAL_HEADERS_COMPILE_AS_CXX)
    message(FATAL_ERROR "Canonical RIFT headers do not compile as C++")
endif()
unset(CMAKE_REQUIRED_INCLUDES)

set(RIFT_CORE_SOURCES
    # Runtime, memory, tokens, and portable regex automaton.
    src/core/platform.c
    src/core/memory.c
    src/core/token.c
    src/core/regex.c
    src/core/bytecode.c
    src/core/rift.c

    # Tokenizer, syntax/parser AST, semantic compiler, and code generation.
    src/lang/lexer.c
    src/lang/parser.c
    src/lang/semantic.c
    src/lang/codegen.c
    src/lang/emitter.c
    src/lang/pattern.c
    src/lang/validator.c

    # Linker/codec and RIFT-0 -> RIFT-1 bridge.
    src/lang/codec.c
    src/bridge/bridge.c
    src/bridge/ropen.c
    src/bridge/traversal.c
)

set(RIFTLANG_SOURCES
    src/riftlang/riftlang.c
)

set(RIFTTEST_SOURCES
    src/rifttest/rifttest.c
)

function(rift_common_target target)
    target_include_directories(${target}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    set_target_properties(${target} PROPERTIES
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
        ARCHIVE_OUTPUT_DIRECTORY "${RIFT_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${RIFT_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${RIFT_OUTPUT_DIR}"
    )
    foreach(config RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(${target} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${config} "${RIFT_OUTPUT_DIR}"
            LIBRARY_OUTPUT_DIRECTORY_${config} "${RIFT_OUTPUT_DIR}"
            RUNTIME_OUTPUT_DIRECTORY_${config} "${RIFT_OUTPUT_DIR}"
        )
    endforeach()
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${RIFT_OUTPUT_DIR}/debug"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RIFT_OUTPUT_DIR}/debug"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RIFT_OUTPUT_DIR}/debug"
    )
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_library(rift STATIC ${RIFT_CORE_SOURCES})
set_target_properties(rift PROPERTIES OUTPUT_NAME rift PREFIX "" EXPORT_NAME rift)
rift_common_target(rift)
add_library(RIFT::rift ALIAS rift)

add_library(rift_shared SHARED ${RIFT_CORE_SOURCES})
set_target_properties(rift_shared PROPERTIES
    OUTPUT_NAME rift
    PREFIX ""
    EXPORT_NAME rift_shared
)
if(WIN32)
    set_target_properties(rift_shared PROPERTIES ARCHIVE_OUTPUT_NAME rift_import)
endif()
target_compile_definitions(rift_shared PRIVATE RIFT_CORE_BUILD_DLL)
target_compile_definitions(rift_shared INTERFACE RIFT_CORE_USE_DLL)
rift_common_target(rift_shared)
add_library(RIFT::rift_shared ALIAS rift_shared)

add_library(riftlang STATIC ${RIFTLANG_SOURCES})
set_target_properties(riftlang PROPERTIES OUTPUT_NAME riftlang PREFIX "" EXPORT_NAME riftlang)
target_link_libraries(riftlang PUBLIC rift)
rift_common_target(riftlang)
add_library(RIFT::riftlang ALIAS riftlang)

add_library(riftlang_shared SHARED ${RIFTLANG_SOURCES})
set_target_properties(riftlang_shared PROPERTIES
    OUTPUT_NAME riftlang
    PREFIX ""
    EXPORT_NAME riftlang_shared
)
if(WIN32)
    set_target_properties(riftlang_shared PROPERTIES ARCHIVE_OUTPUT_NAME riftlang_import)
endif()
target_compile_definitions(riftlang_shared PRIVATE
    RIFTLANG_BUILD_DLL
    RIFT_CORE_USE_DLL
)
target_compile_definitions(riftlang_shared INTERFACE
    RIFTLANG_USE_DLL
    RIFT_CORE_USE_DLL
)
target_link_libraries(riftlang_shared PUBLIC rift_shared)
rift_common_target(riftlang_shared)
add_library(RIFT::riftlang_shared ALIAS riftlang_shared)

add_library(rifttest STATIC ${RIFTTEST_SOURCES})
set_target_properties(rifttest PROPERTIES OUTPUT_NAME rifttest PREFIX "" EXPORT_NAME rifttest)
target_link_libraries(rifttest PUBLIC rift)
rift_common_target(rifttest)
add_library(RIFT::rifttest ALIAS rifttest)

add_library(rifttest_shared SHARED ${RIFTTEST_SOURCES})
set_target_properties(rifttest_shared PROPERTIES
    OUTPUT_NAME rifttest
    PREFIX ""
    EXPORT_NAME rifttest_shared
)
if(WIN32)
    set_target_properties(rifttest_shared PROPERTIES ARCHIVE_OUTPUT_NAME rifttest_import)
endif()
target_compile_definitions(rifttest_shared PRIVATE
    RIFTTEST_BUILD_DLL
    RIFT_CORE_USE_DLL
)
target_compile_definitions(rifttest_shared INTERFACE
    RIFTTEST_USE_DLL
    RIFT_CORE_USE_DLL
)
target_link_libraries(rifttest_shared PUBLIC rift_shared)
rift_common_target(rifttest_shared)
add_library(RIFT::rifttest_shared ALIAS rifttest_shared)

if(UNIX)
    add_custom_command(TARGET rift POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/librift.a"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rift> "${RIFT_OUTPUT_DIR}/librift.a")
    add_custom_command(TARGET rift_shared POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/librift${CMAKE_SHARED_LIBRARY_SUFFIX}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rift_shared> "${RIFT_OUTPUT_DIR}/librift${CMAKE_SHARED_LIBRARY_SUFFIX}")
    add_custom_command(TARGET riftlang POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/libriftlang.a"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:riftlang> "${RIFT_OUTPUT_DIR}/libriftlang.a")
    add_custom_command(TARGET riftlang_shared POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/libriftlang${CMAKE_SHARED_LIBRARY_SUFFIX}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:riftlang_shared> "${RIFT_OUTPUT_DIR}/libriftlang${CMAKE_SHARED_LIBRARY_SUFFIX}")
    add_custom_command(TARGET rifttest POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/librifttest.a"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rifttest> "${RIFT_OUTPUT_DIR}/librifttest.a")
    add_custom_command(TARGET rifttest_shared POST_BUILD
        BYPRODUCTS "${RIFT_OUTPUT_DIR}/librifttest${CMAKE_SHARED_LIBRARY_SUFFIX}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rifttest_shared> "${RIFT_OUTPUT_DIR}/librifttest${CMAKE_SHARED_LIBRARY_SUFFIX}")
elseif(MINGW)
    foreach(component rift riftlang rifttest)
        add_custom_command(TARGET ${component} POST_BUILD
            BYPRODUCTS "${RIFT_OUTPUT_DIR}/lib${component}.a"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${component}> "${RIFT_OUTPUT_DIR}/lib${component}.a")
        add_custom_command(TARGET ${component}_shared POST_BUILD
            BYPRODUCTS "${RIFT_OUTPUT_DIR}/lib${component}.dll.a"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_LINKER_FILE:${component}_shared>
                "${RIFT_OUTPUT_DIR}/lib${component}.dll.a")
    endforeach()
elseif(MSVC)
    foreach(component rift riftlang rifttest)
        add_custom_command(TARGET ${component} POST_BUILD
            BYPRODUCTS "${RIFT_OUTPUT_DIR}/lib${component}.lib"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${component}> "${RIFT_OUTPUT_DIR}/lib${component}.lib")
    endforeach()
endif()

set(RIFT_CLI_SOURCES
    src/cli/main.c
    src/cli/dispatch.c
    src/cli/cmd_bridge.c
    src/cli/cmd_compile.c
    src/cli/cmd_doctor.c
    src/cli/cmd_emit.c
    src/cli/cmd_lang.c
    src/cli/cmd_man.c
    src/cli/cmd_parse.c
    src/cli/cmd_raf.c
    src/cli/cmd_test.c
    src/cli/cmd_tokenize.c
)

add_executable(rift_cli ${RIFT_CLI_SOURCES})
set_target_properties(rift_cli PROPERTIES OUTPUT_NAME rift EXPORT_NAME cli)
target_link_libraries(rift_cli PRIVATE rift riftlang rifttest)
rift_common_target(rift_cli)

add_executable(riftlang_cli src/riftlang/main.c)
set_target_properties(riftlang_cli PROPERTIES OUTPUT_NAME riftlang EXPORT_NAME lang_cli)
target_link_libraries(riftlang_cli PRIVATE riftlang)
rift_common_target(riftlang_cli)

if(RIFT_BUILD_EXAMPLES)
    set_source_files_properties(
        examples/floyd/string_pattern_detector.c
        PROPERTIES LANGUAGE CXX
    )
    add_executable(string_pattern_detector examples/floyd/string_pattern_detector.c)
    target_link_libraries(string_pattern_detector PRIVATE rift riftlang rifttest)
    rift_common_target(string_pattern_detector)
endif()

if(RIFT_BUILD_TESTS)
    enable_testing()
    set(RIFT_TESTS
        test_token
        test_lexer
        test_codec
        test_regex
        test_bytecode
        test_bridge
        test_riftlang
        test_rifttest
    )
    foreach(test_name IN LISTS RIFT_TESTS)
        add_executable(${test_name} "tests/${test_name}.c")
        target_link_libraries(${test_name} PRIVATE rift riftlang rifttest)
        target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()

    add_executable(test_headers_c tests/test_headers.c)
    target_link_libraries(test_headers_c PRIVATE rift riftlang rifttest)
    add_test(NAME test_headers_c COMMAND test_headers_c)

    add_executable(test_headers_cpp tests/test_headers.cpp)
    target_link_libraries(test_headers_cpp PRIVATE rift riftlang rifttest)
    add_test(NAME test_headers_cpp COMMAND test_headers_cpp)

    add_executable(test_shared_consumer tests/test_shared_consumer.c)
    target_link_libraries(test_shared_consumer PRIVATE
        rift_shared riftlang_shared rifttest_shared)
    rift_common_target(test_shared_consumer)
    add_test(NAME test_shared_consumer COMMAND test_shared_consumer)

    add_test(NAME rift_cli_version COMMAND rift_cli --version)
    add_test(NAME rift_cli_tests COMMAND rift_cli test)
    add_test(NAME rift_cli_compile
        COMMAND rift_cli compile
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/smoke.rift"
            "${CMAKE_CURRENT_BINARY_DIR}/smoke-rift.c")
    add_test(NAME riftlang_cli_validate
        COMMAND riftlang_cli --validate
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/smoke.rift")
    add_test(NAME riftlang_cli_export
        COMMAND riftlang_cli
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/smoke.rift"
            -o "${CMAKE_CURRENT_BINARY_DIR}/smoke-riftlang.c")
endif()

install(TARGETS
        rift rift_shared
        riftlang riftlang_shared
        rifttest rifttest_shared
        rift_cli riftlang_cli
    EXPORT RIFTTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/rift DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES include/rift.h include/riftlang.h include/rifttest.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/RIFTConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
    cmake/RIFTConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/RIFTConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RIFT"
)
install(EXPORT RIFTTargets
    FILE RIFTTargets.cmake
    NAMESPACE RIFT::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RIFT"
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RIFTConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RIFTConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RIFT"
)
