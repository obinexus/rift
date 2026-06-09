# FindCheck.cmake - Find the Check unit testing framework
#
# This module defines:
#  CHECK_INCLUDE_DIRS - where to find check.h
#  CHECK_LIBRARIES - the libraries to link against
#  CHECK_FOUND - if the library was found

find_path(CHECK_INCLUDE_DIR NAMES check.h)
find_library(CHECK_LIBRARY NAMES check)

# Check might require pthread
find_package(Threads)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Check DEFAULT_MSG
                                  CHECK_LIBRARY CHECK_INCLUDE_DIR)

if(CHECK_FOUND)
  set(CHECK_LIBRARIES ${CHECK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
  set(CHECK_INCLUDE_DIRS ${CHECK_INCLUDE_DIR})
endif()

mark_as_advanced(CHECK_INCLUDE_DIR CHECK_LIBRARY)