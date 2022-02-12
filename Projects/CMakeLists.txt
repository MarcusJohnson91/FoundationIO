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
set(FoundationIO_Tests_Dir     "${FoundationIO_Library_Dir}/src/Tests" CACHE PATH INTERNAL FORCE)
set(FoundationIO_TextIO_Tests  "${FoundationIO_Library_Dir}/src/TextIO/Tests" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Resources_Dir "${FoundationIO_Root_Dir}/Resources" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Build_Dir     "${FoundationIO_Root_Dir}/BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(FoundationIO_Projects_Dir  "${FoundationIO_Root_Dir}/Projects")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FoundationIO_Build_Dir}/bin)

set(FoundationIO_VERSION      "")
include(${FoundationIO_Projects_Dir}/GetVersion.cmake)
GetVersionString(${FoundationIO_Library_Dir}/FoundationIO.h FoundationIO_VERSION)

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
    enable_testing()
    include(${FoundationIO_Projects_Dir}/RegisterTests.cmake)

    add_custom_target(FoundationIO_Tests Test DEPENDS ${FoundationIO_TEST_TARGETS})

    RegisterTest(${FoundationIO_Tests_Dir} AsynchronousIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} BufferIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} CryptographyIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} FileIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} GUUID FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} MathIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} NetworkIO FoundationIO)
    RegisterTest(${FoundationIO_Tests_Dir} PlatformIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} CommandLineIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} FormatIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} LocalizationIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} LogIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} StringIO FoundationIO)
    RegisterTest(${FoundationIO_TextIO_Tests} StringSetIO FoundationIO)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
