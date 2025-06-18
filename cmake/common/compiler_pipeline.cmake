# =================================================================
# RIFT Common Build Pipeline Infrastructure
# OBINexus Computing Framework - AEGIS Methodology
# =================================================================

# RIFT pipeline stage macro
macro(add_rift_stage STAGE_NAME COMPONENT_NAME)
    # Collect source files
    file(GLOB_RECURSE STAGE_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c"
    )
    
    file(GLOB_RECURSE STAGE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${STAGE_NAME}/core/*.h"
    )
    
    # Only create targets if source files exist
    if(STAGE_SOURCES)
        # Create static library
        add_library(${STAGE_NAME}_static STATIC ${STAGE_SOURCES})
        target_include_directories(${STAGE_NAME}_static PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        
        # Link dependencies
        target_link_libraries(${STAGE_NAME}_static 
            OpenSSL::SSL OpenSSL::Crypto Threads::Threads
        )
        
        # Set output directories
        set_target_properties(${STAGE_NAME}_static PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
        )
        
        # Installation
        install(TARGETS ${STAGE_NAME}_static
            ARCHIVE DESTINATION lib
            COMPONENT static_libraries
        )
        
        message(STATUS "Added RIFT stage: ${STAGE_NAME} (${COMPONENT_NAME})")
    else()
        message(WARNING "No source files found for ${STAGE_NAME}")
    endif()
    
    # Install headers if they exist
    if(STAGE_HEADERS)
        install(FILES ${STAGE_HEADERS}
            DESTINATION include/rift/${COMPONENT_NAME}
            COMPONENT headers
        )
    endif()
endmacro()

# RIFT validation target
function(add_rift_validation STAGE_NAME)
    add_custom_command(TARGET ${STAGE_NAME}_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Validating stage ${STAGE_NAME} artifacts..."
        COMMAND test -f "$<TARGET_FILE:${STAGE_NAME}_static>"
        COMMENT "AEGIS artifact validation for stage ${STAGE_NAME}"
    )
endfunction()
