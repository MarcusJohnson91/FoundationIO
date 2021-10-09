cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
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

include(DetectArchitecture.cmake)
FindTargetArchitecture(TargetArchitecture)

set(BASE_DIRECTORY      "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH INTERNAL FORCE)
set(LIBRARY_DIRECTORY   "${BASE_DIRECTORY}/Library" CACHE PATH INTERNAL FORCE)
set(TESTSRC_DIRECTORY   "${BASE_DIRECTORY}/Tests" CACHE PATH INTERNAL FORCE)
set(RESOURCE_DIRECTORY  "${BASE_DIRECTORY}/Resources" CACHE PATH INTERNAL FORCE)
set(VERSION_DIRECTORY   "${BASE_DIRECTORY}/Library/include/Private/Version" CACHE PATH INTERNAL FORCE)
set(VERSION_STRING      "")
set(BUILD_DIRECTORY     "${BASE_DIRECTORY}/BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")

include(GetVersion.cmake)
GetVersionString(${VERSION_DIRECTORY} VERSION_STRING)

if(CMAKE_HOST_UNIX AND (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))
    message("We're checking if TextIOTables.h should be updated, it could take a few seconds to about 12 minutes.")
    execute_process(
    COMMAND sh "${LIBRARY_DIRECTORY}/include/TextIO/Private/TextIOTables_Create.sh"                                              "${LIBRARY_DIRECTORY}/include/TextIO/Private/TextIOTables.h"
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

configure_file(../Resources/FoundationIO.pc.in "${BUILD_DIRECTORY}/Resources/FoundationIO.pc")

set(LIBRARY_SOURCE
"${LIBRARY_DIRECTORY}/src/AsynchronousIO.c"
"${LIBRARY_DIRECTORY}/src/BufferIO.c"
"${LIBRARY_DIRECTORY}/src/CryptographyIO.c"
"${LIBRARY_DIRECTORY}/src/FileIO.c"
"${LIBRARY_DIRECTORY}/src/MathIO.c"
"${LIBRARY_DIRECTORY}/src/NetworkIO.c"
"${LIBRARY_DIRECTORY}/src/PlatformIO.c"
"${LIBRARY_DIRECTORY}/src/TextIO/TextIOTypes.c"
"${LIBRARY_DIRECTORY}/src/TextIO/CommandLineIO.c"
"${LIBRARY_DIRECTORY}/src/TextIO/FormatIO.c"
"${LIBRARY_DIRECTORY}/src/TextIO/LocalizationIO.c"
"${LIBRARY_DIRECTORY}/src/TextIO/LogIO.c"
"${LIBRARY_DIRECTORY}/src/TextIO/StringIO.c"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(LIBRARY_SOURCE "${LIBRARY_SOURCE}" "${LIBRARY_DIRECTORY}/src/TestIO.c") #Only compile TestIO into FoundationIO if we're in debug mode and linking the test programs
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")


set(PRIVATE_SOURCE
"${LIBRARY_DIRECTORY}/src/TextIO/Private/TextIOTables.c"
)

option(BUILD_AS_SHARED_LIBRARY "Should FoundationIO be compiled as a dynamic library (.dylib/.dll/.so)?" ON)

if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(FoundationIO SHARED ${LIBRARY_SOURCE} ${PRIVATE_SOURCE})
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(FoundationIO STATIC ${LIBRARY_SOURCE} ${PRIVATE_SOURCE})
endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

if(NOT WIN32)
  string(REGEX MATCH "^([0-9]+)\.([0-9]+)\.([0-9]+)\:([0-9a-fA-F]+)$" MATCH_STRING ${VERSION_STRING})
  set("VERSION_MAJOR" ${CMAKE_MATCH_1})
  set("VERSION_MINOR" ${CMAKE_MATCH_2})
  set("VERSION_PATCH" ${CMAKE_MATCH_3})
  set(SANITIZED_VERSION "")
  string(CONCAT SANITIZED_VERSION "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}")
  set_target_properties(FoundationIO PROPERTIES VERSION ${SANITIZED_VERSION})
endif(NOT WIN32)

if(CMAKE_HOST_WIN32)
    target_link_libraries(FoundationIO ws2_32 bcrypt)
endif(CMAKE_HOST_WIN32)

set_target_properties(FoundationIO PROPERTIES PREFIX "")

target_include_directories(FoundationIO PUBLIC "${LIBRARY_DIRECTORY}/include")
target_include_directories(FoundationIO PUBLIC "${LIBRARY_DIRECTORY}/include/TextIO")

if(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local/FoundationIO") # and lets alias the executables and library back to the normal /usr/local location
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_WIN32)
    set(CMAKE_INSTALL_PREFIX "%ProgramFiles%\FoundationIO")
endif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)

install(TARGETS FoundationIO                                 DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY "${BUILD_DIRECTORY}/include"               DESTINATION "${CMAKE_INSTALL_PREFIX}/include" FILES_MATCHING PATTERN "*.h" PATTERN "*Private*" EXCLUDE)
install(FILES "${BUILD_DIRECTORY}/Resources/FoundationIO.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    macro(register_test Name)
        string(CONCAT Test_Name "Test_${Name}")

        add_executable(${Test_Name} "${TESTSRC_DIRECTORY}/${Test_Name}.c")

        target_link_libraries(${Test_Name} FoundationIO)

        set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${Test_Name})

        add_test(${Test_Name} "${CMAKE_CURRENT_BINARY_DIR}/${Test_Name}")

        set_tests_properties(${Test_Name} PROPERTIES TIMEOUT 15)

    endmacro(register_test)

    register_test(AsynchronousIO)
    register_test(BufferIO)
    register_test(CryptographyIO)
    register_test(FileIO)
    register_test(MathIO)
    register_test(NetworkIO)
    register_test(PlatformIO)
    register_test(CommandLineIO)
    register_test(FormatIO)
    register_test(LocalizationIO)
    register_test(LogIO)
    register_test(StringIO)

    add_custom_target(All_Tests ALL DEPENDS ${UNIT_TEST_TARGETS})
    add_custom_command(TARGET All_Tests COMMENT "Run tests" POST_BUILD COMMAND ctest ARGS --output-on-failure WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

if($<C_COMPILER_ID:AppleClang,ARMClang,XLClang,Clang>)

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fsanitize=address,undefined -fno-omit-frame-pointer")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-ferror-limit=1024 \
-fPIC \
-fshort-enums \
-funsigned-char \
-std=c2x \
-Weverything \
-Wall \
-Wextra \
-Wpedantic \
-Wno-assign-enum \
-Wno-c99-compat \
-Wno-ignored-attributes \
-Wno-incompatible-pointer-types-discards-qualifiers \
-Wno-padded \
-Wno-pointer-sign \
-Wno-reserved-id-macro \
-Wno-sign-conversion \
-Wreserved-identifier \
-Wenum-conversion \
-Wswitch-enum \
-Bsymbolic \
-fno-semantic-interposition \
-fvisibility-inlines-hidden \
-foptimize-sibling-calls \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-fno-optimize-sibling-calls \
-fstack-check \
-DDEBUG=1 \
-g3 \
-O1 \
-glldb \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
-Ofast \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-fno-optimize-sibling-calls \
-fstack-check \
-DDEBUG=1 \
-g \
-Ofast \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
-Oz \
")

set(LINK_FLAGS ${LINK_FLAGS} " \
-flto=full \
-lm \
")

if(CMAKE_HOST_APPLE)
set(LINK_FLAGS ${LINK_FLAGS} " \
-dead_strip_dylibs
")
endif(CMAKE_HOST_APPLE)

set(LINK_FLAGS_DEBUG ${LINK_FLAGS} " \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
")

set(LINK_FLAGS_RELWITHDEBINFO ${LINK_FLAGS} " \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
")

set(LINK_FLAGS_RELEASE ${LINK_FLAGS} " \
")

set(LINK_FLAGS_MINSIZEREL ${LINK_FLAGS} " \
")

elseif($<C_COMPILER_ID:GNU>)

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fshort-enums \
-funroll-loops \
-funsigned-char \
-std=c2x \
-Wall \
-Wextra \
-Wpedantic \
-Wno-ignored-attributes \
-Wno-padded \
-Wno-pointer-sign \
-Wno-sign-conversion \
-Wno-varargs \
-Wsuggest-attribute=const \
-Wsuggest-attribute=pure \
-Wsuggest-attribute=malloc \
-Wsuggest-attribute=noreturn \
-Wsuggest-attribute=format \
-Wmissing-format-attribute \
-Wmissing-noreturn \
-Wswitch-enum \
-Wreserved-identifier \
-Bsymbolic \
-fno-semantic-interposition \
-fvisibility-inlines-hidden \
-foptimize-sibling-calls \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fno-omit-frame-pointer \
-fsanitize=address,undefined \
-fstack-check \
-DDEBUG=1 \
-g \
-O1 \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
-Ofast \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
-fno-omit-frame-pointer \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-fstack-check \
-DDEBUG=1 \
-g \
-Ofast \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
-Os \
")

set(LINK_FLAGS ${LINK_FLAGS} " \
-flto=full \
-lm \
")

if(CMAKE_HOST_APPLE)
set(LINK_FLAGS ${LINK_FLAGS} " \
-dead_strip_dylibs
")
endif(CMAKE_HOST_APPLE)

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
-fno-omit-frame-pointer \
-fsanitize=address,undefined \
")

set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
-fno-omit-frame-pointer \
-fsanitize=address,undefined \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
")

elseif($<C_COMPILER_ID:MSVC>)

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
/bigobj \
/FAu \
/GF \
/Gm- \
/GT \
/Gy \
/JMC \
/J \
/MP \
/MT \
/Ob2 \
/openmp \
/p:CharacterSet=Unicode \
/p:FunctionLevelLinking=true \
/p:PrecompiledHeader=NotUsing \
/p:RuntimeLibrary=MultiThreaded \
/p:UseOfMfc=false \
/p:WarningLevel=Level3 \
/Qpar \
/sdl- \
/std:c++17 \
/W3 \
/Zc:rvalueCast \
/Zc:wchar_t \
/Wall \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
/DDEBUG=1 \
/D_DEBUG=1 \
/p:UseDebugLibraries=true \
/O1 \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
/p:UseDebugLibraries=false \
/Ox \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
/DDEBUG=1 \
/D_DEBUG=1 \
/p:UseDebugLibraries=true \
-Ox \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
/p:UseDebugLibraries=false \
-Os \
")

set(LINK_FLAGS " \
${LINK_FLAGS} \
/GL \
/LTCG:INCREMENTAL
/OPT:REF
/flto \
")

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
/DEBUG \
")


set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
/DEBUG \
/RELEASE \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
/RELEASE \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
/RELEASE \
")

endif($<C_COMPILER_ID:AppleClang,ARMClang,XLClang,Clang>)
