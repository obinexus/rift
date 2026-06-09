#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibRift::LibRift" for configuration "Debug"
set_property(TARGET LibRift::LibRift APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibRift::LibRift PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librift.a"
  )

list(APPEND _cmake_import_check_targets LibRift::LibRift )
list(APPEND _cmake_import_check_files_for_LibRift::LibRift "${_IMPORT_PREFIX}/lib/librift.a" )

# Import target "LibRift::librift_core" for configuration "Debug"
set_property(TARGET LibRift::librift_core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibRift::librift_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librift_core.a"
  )

list(APPEND _cmake_import_check_targets LibRift::librift_core )
list(APPEND _cmake_import_check_files_for_LibRift::librift_core "${_IMPORT_PREFIX}/lib/librift_core.a" )

# Import target "LibRift::librift_regex" for configuration "Debug"
set_property(TARGET LibRift::librift_regex APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibRift::librift_regex PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librift_regex.a"
  )

list(APPEND _cmake_import_check_targets LibRift::librift_regex )
list(APPEND _cmake_import_check_files_for_LibRift::librift_regex "${_IMPORT_PREFIX}/lib/librift_regex.a" )

# Import target "LibRift::rift-cli" for configuration "Debug"
set_property(TARGET LibRift::rift-cli APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibRift::rift-cli PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/rift"
  )

list(APPEND _cmake_import_check_targets LibRift::rift-cli )
list(APPEND _cmake_import_check_files_for_LibRift::rift-cli "${_IMPORT_PREFIX}/bin/rift" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
