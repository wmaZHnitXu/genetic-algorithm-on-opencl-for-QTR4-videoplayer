#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "pcre2::pcre2-8-shared" for configuration "Release"
set_property(TARGET pcre2::pcre2-8-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(pcre2::pcre2-8-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/pcre2-8.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/pcre2-8.dll"
  )

list(APPEND _cmake_import_check_targets pcre2::pcre2-8-shared )
list(APPEND _cmake_import_check_files_for_pcre2::pcre2-8-shared "${_IMPORT_PREFIX}/lib/pcre2-8.lib" "${_IMPORT_PREFIX}/bin/pcre2-8.dll" )

# Import target "pcre2::pcre2-posix-shared" for configuration "Release"
set_property(TARGET pcre2::pcre2-posix-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(pcre2::pcre2-posix-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/pcre2-posix.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/pcre2-posix.dll"
  )

list(APPEND _cmake_import_check_targets pcre2::pcre2-posix-shared )
list(APPEND _cmake_import_check_files_for_pcre2::pcre2-posix-shared "${_IMPORT_PREFIX}/lib/pcre2-posix.lib" "${_IMPORT_PREFIX}/bin/pcre2-posix.dll" )

# Import target "pcre2::pcre2-16-shared" for configuration "Release"
set_property(TARGET pcre2::pcre2-16-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(pcre2::pcre2-16-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/pcre2-16.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/pcre2-16.dll"
  )

list(APPEND _cmake_import_check_targets pcre2::pcre2-16-shared )
list(APPEND _cmake_import_check_files_for_pcre2::pcre2-16-shared "${_IMPORT_PREFIX}/lib/pcre2-16.lib" "${_IMPORT_PREFIX}/bin/pcre2-16.dll" )

# Import target "pcre2::pcre2-32-shared" for configuration "Release"
set_property(TARGET pcre2::pcre2-32-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(pcre2::pcre2-32-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/pcre2-32.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/pcre2-32.dll"
  )

list(APPEND _cmake_import_check_targets pcre2::pcre2-32-shared )
list(APPEND _cmake_import_check_files_for_pcre2::pcre2-32-shared "${_IMPORT_PREFIX}/lib/pcre2-32.lib" "${_IMPORT_PREFIX}/bin/pcre2-32.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
