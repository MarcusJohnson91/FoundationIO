cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(FoundationIO LANGUAGES C)

if(NOT DEFINED CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif(NOT DEFINED CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions(-DPlatformIO_LoggingStatus=PlatformIO_EnableLogging)
endif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "Debug")

set(FoundationIO_Projects_Dir  "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Root_Dir      "${FoundationIO_Projects_Dir}/../" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Library_Dir   "${FoundationIO_Root_Dir}/Library" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Tests_Dir     "${FoundationIO_Library_Dir}/src/Tests" CACHE PATH INTERNAL FORCE)
set(FoundationIO_TextIO_Tests  "${FoundationIO_Library_Dir}/src/TextIO/Tests" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Resources_Dir "${FoundationIO_Root_Dir}/Resources" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Build_Dir     "${FoundationIO_Root_Dir}/BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")

option(BUILD_AS_SHARED_LIBRARY "Should FoundationIO be compiled as a dynamic library? (.dylib/.dll/.so)" ON)

if(BUILD_AS_SHARED_LIBRARY EQUAL TRUE)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(LibraryExtension "dylib")
        set(FoundationIOName FoundationIO_Shared)
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(LibraryExtension "dll")
        set(FoundationIOName FoundationIO_Shared)
    else(CMAKE_HOST_SYSTEM_NAME NOT STREQUAL "Darwin" AND CMAKE_HOST_SYSTEM_NAME NOT STREQUAL "Windows")
        set(FoundationIOName FoundationIO_Shared)
        set(LibraryExtension "so")
    endif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
elseif(BUILD_AS_SHARED_LIBRARY EQUAL FALSE)
    set(FoundationIOName FoundationIO_Static)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(LibraryExtension "lib")
    elseif(CMAKE_HOST_SYSTEM_NAME NOT STREQUAL "Windows")
        set(LibraryExtension "a")
    endif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
endif(BUILD_AS_SHARED_LIBRARY EQUAL TRUE)

if(IS_DIRECTORY "../.git")
set(CMAKE_OUTPUT_DIRECTORY         "${FoundationIO_Build_Dir}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FoundationIO_Build_Dir}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FoundationIO_Build_Dir}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FoundationIO_Build_Dir}/bin")
endif(IS_DIRECTORY "../.git")

set(FoundationIO_VERSION "")
include(${FoundationIO_Projects_Dir}/GetVersion.cmake)
GetVersionString(${FoundationIO_Library_Dir}/FoundationIO.h FoundationIO_VERSION)

include(${FoundationIO_Projects_Dir}/BuildSettings.cmake)

if(CMAKE_HOST_UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))
    message("We're checking if TextIOTables.h should be updated, it will take between a few seconds and 5 minutes.")
    execute_process(COMMAND sh "${FoundationIO_Projects_Dir}/TextIOTables_Create.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE TextIOTables_Create_Status 
    RESULTS_VARIABLE STDOUT
    ERROR_VARIABLE STDERR)
endif(CMAKE_HOST_UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))

if(IS_DIRECTORY "../.git")
configure_file("${FoundationIO_Resources_Dir}/FoundationIO.pc.in" "${CMAKE_OUTPUT_DIRECTORY}/Resources/FoundationIO.pc")
endif(IS_DIRECTORY "../.git")

set(FoundationIO_SOURCE
    "${FoundationIO_Library_Dir}/src/AssertIO.c"
    "${FoundationIO_Library_Dir}/src/AsynchronousIO.c"
    "${FoundationIO_Library_Dir}/src/BufferIO.c"
    "${FoundationIO_Library_Dir}/src/CollectionIO.c"
    "${FoundationIO_Library_Dir}/src/CryptographyIO.c"
    "${FoundationIO_Library_Dir}/src/FileIO.c"
    "${FoundationIO_Library_Dir}/src/GUUID.c"
    "${FoundationIO_Library_Dir}/src/MathIO.c"
    "${FoundationIO_Library_Dir}/src/NetworkIO.c"
    "${FoundationIO_Library_Dir}/src/PlatformIO.c"
    "${FoundationIO_Library_Dir}/src/SortIO.c"
    "${FoundationIO_Library_Dir}/src/TestIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/CommandLineIO.c"
    "${FoundationIO_Library_Dir}/src/ExtensibleIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/FormatIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/LocalizationIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/LogIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/StringIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/StringSetIO.c"
    "${FoundationIO_Library_Dir}/src/TextIO/TextIOTypes.c"
    "${FoundationIO_Library_Dir}/src/TextIO/Private/TextIOTables.c"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(FOUNDATIONIO_SOURCE "${FoundationIO_SOURCE}" "${FoundationIO_Library_Dir}/src/TestIO.c") #Only compile TestIO into FoundationIO if we're in debug mode and linking the test programs
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(FoundationIO_Static STATIC ${FOUNDATIONIO_SOURCE})
    add_library(FoundationIO_Shared SHARED ${FOUNDATIONIO_SOURCE})
    
   if(CMAKE_HOST_WIN32)
target_link_libraries(FoundationIO_Static ws2_32 bcrypt)
   target_link_libraries(FoundationIO_Shared ws2_32 bcrypt)
   else(NOT CMAKE_HOST_WIN32)
       set_target_properties(FoundationIO_Shared PROPERTIES PREFIX "")
set_target_properties(FoundationIO_Static PROPERTIES PREFIX "")
   endif(CMAKE_HOST_WIN32)
    
    
    
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(FoundationIO_Static STATIC ${FOUNDATIONIO_SOURCE})
    
       if(CMAKE_HOST_WIN32)
target_link_libraries(FoundationIO_Static ws2_32 bcrypt)
   endif(CMAKE_HOST_WIN32)

endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include")
target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include/TextIO")

if(IS_DIRECTORY "../.git")
    if(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
        set(CMAKE_INSTALL_PREFIX "/usr/local/FoundationIO")
    elseif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_WIN32)
        set(CMAKE_INSTALL_PREFIX "%ProgramFiles%\FoundationIO")
    endif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
endif(IS_DIRECTORY "../.git")

install(TARGETS FoundationIO                                        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY "${FoundationIO_Build_Dir}/include"               DESTINATION "${CMAKE_INSTALL_PREFIX}/include" FILES_MATCHING PATTERN "*.h" PATTERN "*Private*" EXCLUDE)
install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/FoundationIO.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    include(CTest)
    enable_testing()
    include(${FoundationIO_Projects_Dir}/RegisterTests.cmake)
    add_custom_target(FoundationIO_Tests Test DEPENDS ${FoundationIO_TEST_TARGETS})

    RegisterTest(${FoundationIO_Tests_Dir} AsynchronousIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} BufferIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} CollectionIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} CryptographyIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} FileIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} GUUID FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} MathIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} NetworkIO FoundationIOName)
    RegisterTest(${FoundationIO_Tests_Dir} PlatformIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} CommandLineIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} ExtensibleIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} FormatIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} LocalizationIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} LogIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} StringIO FoundationIOName)
    RegisterTest(${FoundationIO_TextIO_Tests} StringSetIO FoundationIOName)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
