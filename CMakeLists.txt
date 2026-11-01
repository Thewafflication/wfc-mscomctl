# wfc-mscomctl: Windows Common Controls (mscomctl.ocx, library MSComctlLib).
#
# The member table is generated from the type-library evidence
# (scripts/New-UiControlTables.ps1 in WFC).
# The table is built static by default; WFC_MSCOMCTL_SHARED builds it as
# wfcmscomctl.dll so a program can load it at run time instead. The version is
# the one in VERSION and is stamped into the DLL's version resource.
#
# Parent project variables used when present: WFC_OCX_SHARED (the default of
# WFC_MSCOMCTL_SHARED) and the functions wfc_enable_strict_warnings and
# wsp_enable_hardening.

# Standalone build: the library outside WFC. WFC itself adds this directory
# with add_subdirectory and supplies the project, CTest and the WSP hardening.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  cmake_minimum_required(VERSION 3.28)
  project(wfc-mscomctl LANGUAGES CXX)
  if(WFC_WINELIB)
    # winegcc writes name.exe (a launcher) and name.exe.so; the platform module
    # resets the suffix when the project is declared, so it is set here.
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")
  endif()
  option(WFC_BUILD_TESTS "Build the tests" ON)
  include(CTest)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  include(WfcWarnings)
  # WSP-SEC-0015: the pinned WSP baseline supplies the hardening flags (not for
  # the WebAssembly and Winelib targets, whose toolchains do not take them).
  if(
    EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/wsp/tools/cmake/WspBuild.cmake"
    AND NOT EMSCRIPTEN
    AND NOT WFC_WINELIB
  )
    include("${CMAKE_CURRENT_SOURCE_DIR}/wsp/tools/cmake/WspBuild.cmake")
  endif()
endif()

if(NOT DEFINED WFC_OCX_SHARED)
  set(WFC_OCX_SHARED OFF)
endif()
option(
  WFC_MSCOMCTL_SHARED
  "Build the WFC mscomctl library as wfcmscomctl.dll"
  ${WFC_OCX_SHARED}
)

file(
  STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
  WFC_MSCOMCTL_VERSION
  LIMIT_COUNT 1
)
set(
  WFC_MSCOMCTL_VERSION
  "${WFC_MSCOMCTL_VERSION}"
  CACHE INTERNAL
  "wfcmscomctl version"
)

set(WFC_MSCOMCTL_SOURCES src/mscomctl_tables.cpp)

if(WFC_MSCOMCTL_SHARED AND WIN32)
  enable_language(RC)
  string(REPLACE "." ";" version_parts "${WFC_MSCOMCTL_VERSION}")
  list(GET version_parts 0 WFC_LIB_VERSION_MAJOR)
  list(GET version_parts 1 WFC_LIB_VERSION_MINOR)
  list(GET version_parts 2 WFC_LIB_VERSION_PATCH)
  set(WFC_LIB_VERSION "${WFC_MSCOMCTL_VERSION}")
  file(
    STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/TARGET"
    WFC_LIB_TARGET
    LIMIT_COUNT 1
  )
  configure_file(
    src/registration_info.hpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/registration_info.hpp"
    @ONLY
  )
  set(WFC_LIB_NAME wfcmscomctl)
  set(WFC_LIB_DESCRIPTION "WFC Windows Common Controls")
  configure_file(
    version.rc.in
    "${CMAKE_CURRENT_BINARY_DIR}/wfcmscomctl.rc"
    @ONLY
  )
  add_library(
    wfc_mscomctl
    SHARED
    ${WFC_MSCOMCTL_SOURCES}
    src/registration.cpp
    src/wfcmscomctl.def
    "${CMAKE_CURRENT_BINARY_DIR}/wfcmscomctl.rc"
  )
  target_include_directories(wfc_mscomctl PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(wfc_mscomctl PRIVATE advapi32 ole32 oleaut32)
  target_compile_definitions(
    wfc_mscomctl
    PUBLIC WFC_MSCOMCTL_SHARED
    PRIVATE WFC_MSCOMCTL_BUILD_DLL
  )
  set_target_properties(
    wfc_mscomctl
    PROPERTIES
      OUTPUT_NAME wfcmscomctl
      VERSION "${WFC_MSCOMCTL_VERSION}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  )
else()
  add_library(wfc_mscomctl STATIC ${WFC_MSCOMCTL_SOURCES})
  set_target_properties(wfc_mscomctl PROPERTIES OUTPUT_NAME wfcmscomctl_static)
endif()
target_include_directories(
  wfc_mscomctl
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_features(wfc_mscomctl PRIVATE cxx_std_20)
if(COMMAND wfc_enable_strict_warnings)
  wfc_enable_strict_warnings(wfc_mscomctl)
endif()
if(COMMAND wsp_enable_hardening)
  wsp_enable_hardening(wfc_mscomctl)
endif()

# --- Tests --------------------------------------------------------------------
# The table (any build) and, in the shared build, the DLL itself: a C program
# loads wfcmscomctl.dll, checks its COM entry points and version resource, and
# registers and unregisters it in the registry of the current user.
if(BUILD_TESTING AND WFC_BUILD_TESTS)
  add_executable(mscomctl_tables_test tests/mscomctl_tables_test.cpp)
  target_link_libraries(mscomctl_tables_test PRIVATE wfc_mscomctl)
  if(WFC_MSCOMCTL_SHARED)
    # Next to the DLL, so the program finds it.
    set_target_properties(
      mscomctl_tables_test
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
  endif()
  target_compile_features(mscomctl_tables_test PRIVATE cxx_std_20)
  if(COMMAND wfc_enable_strict_warnings)
    wfc_enable_strict_warnings(mscomctl_tables_test)
  endif()
  if(COMMAND wsp_enable_hardening)
    wsp_enable_hardening(mscomctl_tables_test)
  endif()
  add_test(NAME TC-MP0010-mscomctl-tables COMMAND mscomctl_tables_test)
  set_tests_properties(
    TC-MP0010-mscomctl-tables
    PROPERTIES LABELS "unit;mp-0010;mscomctl" TIMEOUT 120
  )

  if(WIN32 AND WFC_MSCOMCTL_SHARED)
    enable_language(C)
    add_executable(mscomctl_dll_test tests/mscomctl_dll_test.c)
    target_link_libraries(mscomctl_dll_test PRIVATE version advapi32)
    add_dependencies(mscomctl_dll_test wfc_mscomctl)
    if(MSVC)
      target_compile_options(mscomctl_dll_test PRIVATE /W4 /WX)
    else()
      target_compile_options(mscomctl_dll_test PRIVATE -Wall -Wextra -Werror)
    endif()
    if(COMMAND wsp_enable_hardening)
      wsp_enable_hardening(mscomctl_dll_test)
    endif()
    add_test(
      NAME TC-MP0010-mscomctl-dll
      COMMAND
        mscomctl_dll_test "$<TARGET_FILE:wfc_mscomctl>"
        "${WFC_MSCOMCTL_VERSION}" "${WFC_LIB_TARGET}"
    )
    set_tests_properties(
      TC-MP0010-mscomctl-dll
      PROPERTIES
        LABELS "integration;mp-0010;mscomctl"
        RESOURCE_LOCK registry
        SKIP_RETURN_CODE 77
    )
  endif()
endif()

# --- WebAssembly demo ---------------------------------------------------------
# With Emscripten the member table also builds as a module and a page that lists
# the classes (web/index.html); CI opens the page in Chromium for the README
# screenshot.
if(EMSCRIPTEN)
  file(
    STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/TARGET"
    WFC_WEB_TARGET
    LIMIT_COUNT 1
  )
  add_executable(mscomctl_web web/wasm_api.cpp)
  target_link_libraries(mscomctl_web PRIVATE wfc_mscomctl)
  target_compile_features(mscomctl_web PRIVATE cxx_std_20)
  target_compile_definitions(
    mscomctl_web
    PRIVATE
      WFC_WEB_VERSION="${WFC_MSCOMCTL_VERSION}"
      WFC_WEB_TARGET="${WFC_WEB_TARGET}"
  )
  set_target_properties(mscomctl_web PROPERTIES OUTPUT_NAME mscomctl SUFFIX .js)
  set(
    WFC_WEB_NAMES
    library_name
    library_version
    library_target
    class_count
    class_name
    member_count
    event_count
    member_name
    event_name
  )
  string(JOIN "','_" WFC_WEB_EXPORTS ${WFC_WEB_NAMES})
  target_link_options(
    mscomctl_web
    PRIVATE
      -sMODULARIZE=1
      -sEXPORT_NAME=create_mscomctl
      "-sEXPORTED_FUNCTIONS=['_${WFC_WEB_EXPORTS}']"
      "-sEXPORTED_RUNTIME_METHODS=['ccall','UTF8ToString']"
      -sENVIRONMENT=web,node
  )
  add_custom_command(
    TARGET mscomctl_web
    POST_BUILD
    COMMAND
      "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/web/index.html"
      "$<TARGET_FILE_DIR:mscomctl_web>/index.html"
  )
endif()
