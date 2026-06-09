#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LibRift::librift_core" for configuration "Debug"
set_property(TARGET LibRift::librift_core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(LibRift::librift_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librift.a"
  )

list(APPEND _cmake_import_check_targets LibRift::librift_core )
list(APPEND _cmake_import_check_files_for_LibRift::librift_core "${_IMPORT_PREFIX}/lib/librift.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
