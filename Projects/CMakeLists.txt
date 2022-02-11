cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(FoundationIO LANGUAGES C)

if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif(NOT DEFINED CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    include(CTest)
    enable_testing()
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
add_compile_definitions(PlatformIO_LoggingStatus=PlatformIO_EnableLogging)
endif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

if(EXISTS "../../Dependencies")
set(FoundationIO_Root_Dir "..")
elseif(NOT EXISTS "../../Dependencies")
set(FoundationIO_Root_Dir "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif(EXISTS "../../Dependencies")

set(FoundationIO_Library_Dir   "${FoundationIO_Root_Dir}/Library" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Resources_Dir "${FoundationIO_Root_Dir}/Resources" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Build_Dir     "${FoundationIO_Root_Dir}/BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(FoundationIO_Projects_Dir  "${FoundationIO_Root_Dir}/Projects")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/bin)

set(VERSION_STRING      "")
include(${FoundationIO_Projects_Dir}/GetVersion.cmake)
GetVersionString("${FoundationIO_Library_Dir}/FoundationIO.h" VERSION_STRING)

include(${FoundationIO_Projects_Dir}/BuildSettings.cmake)

if(CMAKE_HOST_UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))
    message("We're checking if TextIOTables.h should be updated, it will take between a few seconds and 5 minutes.")
    execute_process(
    COMMAND sh "${FoundationIO_Root_Dir}/Projects/TextIOTables_Create.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE TextIOTables_Create_Status 
    RESULTS_VARIABLE STDOUT
    ERROR_VARIABLE STDERR
)
endif(CMAKE_HOST_UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
set(DynlibExt dylib)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
set(DynlibExt dll)
else()
set(DynlibExt so)
endif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")

configure_file("${FoundationIO_Resources_Dir}/FoundationIO.pc.in" "${FoundationIO_Build_Dir}/Resources/FoundationIO.pc")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(LIBRARY_SOURCE "${LIBRARY_SOURCE}" "${FoundationIO_Library_Dir}/src/TestIO.c") #Only compile TestIO into FoundationIO if we're in debug mode and linking the test programs
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

option(BUILD_AS_SHARED_LIBRARY "Should FoundationIO be compiled as a dynamic library? (.dylib/.dll/.so)" ON)

set(FoundationIO_SOURCE
    "${FoundationIO_Library_Dir}/src/AssertIO.c"
    "${FoundationIO_Library_Dir}/src/AsynchronousIO.c"
    "${FoundationIO_Library_Dir}/src/BufferIO.c"
    "${FoundationIO_Library_Dir}/src/CryptographyIO.c"
    "${FoundationIO_Library_Dir}/src/FileIO.c"
    "${FoundationIO_Library_Dir}/src/GUUID.c"
    "${FoundationIO_Library_Dir}/src/MathIO.c"
    "${FoundationIO_Library_Dir}/src/NetworkIO.c"
    "${FoundationIO_Library_Dir}/src/PlatformIO.c"
    "${FoundationIO_Library_Dir}/src/TestIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/CommandLineIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/FormatIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/LocalizationIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/LogIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/StringIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/StringSetIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/TextIOTypes.c"
    "${FoundationIO_Library_Dir}/src/TextIO/Private/TextIOTables.c"
)

if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(FoundationIO SHARED ${FoundationIO_SOURCE})
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(FoundationIO STATIC ${FoundationIO_SOURCE})
endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

if(NOT WIN32)
  string(REGEX MATCH "^([0-9]+)\.([0-9]+)\.([0-9]+)\:([0-9a-fA-F]+)$" MATCH_STRING ${VERSION_STRING})
  set("VERSION_MAJOR" ${CMAKE_MATCH_1})
  set("VERSION_MINOR" ${CMAKE_MATCH_2})
  set("VERSION_PATCH" ${CMAKE_MATCH_3})
  set(SANITIZED_VERSION "")
  string(CONCAT SANITIZED_VERSION "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}")
  set_target_properties(FoundationIO PROPERTIES SOVERSION ${SANITIZED_VERSION})
endif(NOT WIN32)

if(CMAKE_HOST_WIN32)
    target_link_libraries(FoundationIO ws2_32 bcrypt)
endif(CMAKE_HOST_WIN32)

set_target_properties(FoundationIO PROPERTIES PREFIX "")

target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include")
target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include/TextIO")

if(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local/FoundationIO")
    # and lets alias the library back to the normal /usr/local location
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_WIN32)
    set(CMAKE_INSTALL_PREFIX "%ProgramFiles%\FoundationIO")
endif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)

install(TARGETS FoundationIO                                        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY "${FoundationIO_Build_Dir}/include"               DESTINATION "${CMAKE_INSTALL_PREFIX}/include" FILES_MATCHING PATTERN "*.h" PATTERN "*Private*" EXCLUDE)
install(FILES "${FoundationIO_Build_Dir}/Resources/FoundationIO.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    macro(FoundationIO_CreateTest Name) # Path Name
        string(CONCAT Test_Name "Test_${Name}")

        add_executable(${Test_Name} "${FoundationIO_Library_Dir}/${Test_Name}.c")

        target_link_libraries(${Test_Name} FoundationIO)

        set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${Test_Name})

        add_test(${Test_Name} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Test_Name}")

        set_tests_properties(${Test_Name} PROPERTIES TIMEOUT 15)
    endmacro(FoundationIO_CreateTest)

    FoundationIO_CreateTest(AsynchronousIO)
    FoundationIO_CreateTest(BufferIO)
    FoundationIO_CreateTest(CryptographyIO)
    FoundationIO_CreateTest(FileIO)
    FoundationIO_CreateTest(GUUID)
    FoundationIO_CreateTest(MathIO)
    FoundationIO_CreateTest(NetworkIO)
    FoundationIO_CreateTest(PlatformIO)
    FoundationIO_CreateTest(CommandLineIO)
    FoundationIO_CreateTest(FormatIO)
    FoundationIO_CreateTest(LocalizationIO)
    FoundationIO_CreateTest(LogIO)
    FoundationIO_CreateTest(StringIO)
    FoundationIO_CreateTest(StringSetIO)

    add_custom_target(All_Tests Test DEPENDS ${UNIT_TEST_TARGETS})
    add_custom_command(TARGET All_Tests COMMENT "Run tests" POST_BUILD COMMAND ctest ARGS --output-on-failure WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
