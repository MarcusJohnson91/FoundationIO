cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(FoundationIO C)
include(CTest)

if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")

include(DetectArchitecture.cmake)

FindTargetArchitecture(TargetArchitecture)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(FoundationIO_Library_Dir       "${CMAKE_CURRENT_SOURCE_DIR}/../Library")
set(FoundationIO_Test_Dir          "${CMAKE_CURRENT_SOURCE_DIR}/../Tests")
set(FoundationIO_Resource_Dir      "${CMAKE_CURRENT_SOURCE_DIR}/../Resources")

set(FOUNDATIONIO_PUBLIC_HEADERS
"${FoundationIO_Library_Dir}/include/BufferIO.h"
"${FoundationIO_Library_Dir}/include/CryptographyIO.h"
"${FoundationIO_Library_Dir}/include/FileIO.h"
"${FoundationIO_Library_Dir}/include/MathIO.h"
"${FoundationIO_Library_Dir}/include/NetworkIO.h"
"${FoundationIO_Library_Dir}/include/PlatformIO.h"
"${FoundationIO_Library_Dir}/include/TestIO.h"
)

set(TEXTIO_PUBLIC_HEADERS
"${FoundationIO_Library_Dir}/include/TextIO/CommandLineIO.h"
"${FoundationIO_Library_Dir}/include/TextIO/FormatIO.h"
"${FoundationIO_Library_Dir}/include/TextIO/LocalizationIO.h"
"${FoundationIO_Library_Dir}/include/TextIO/LogIO.h"
"${FoundationIO_Library_Dir}/include/TextIO/StringIO.h"
"${FoundationIO_Library_Dir}/include/TextIO/TextIOTypes.h"
)

set(FOUNDATIONIO_SOURCE
"${FoundationIO_Library_Dir}/src/BufferIO.c"
"${FoundationIO_Library_Dir}/src/CryptographyIO.c"
"${FoundationIO_Library_Dir}/src/FileIO.c"
"${FoundationIO_Library_Dir}/src/MathIO.c"
"${FoundationIO_Library_Dir}/src/NetworkIO.c"
"${FoundationIO_Library_Dir}/src/PlatformIO.c"
"${FoundationIO_Library_Dir}/src/TestIO.c"
"${FoundationIO_Library_Dir}/src/TextIO/CommandLineIO.c"
"${FoundationIO_Library_Dir}/src/TextIO/FormatIO.c"
"${FoundationIO_Library_Dir}/src/TextIO/LocalizationIO.c"
"${FoundationIO_Library_Dir}/src/TextIO/LogIO.c"
"${FoundationIO_Library_Dir}/src/TextIO/StringIO.c"
)

if(CMAKE_HOST_UNIX)
    add_definitions(-D_LARGEFILE_SOURCE)
    add_definitions(-D_LARGEFILE64_SOURCE)
    add_definitions(-D_FILE_OFFSET_BITS=64)
    add_definitions(-D__STDC_WANT_LIB_EXT1__)
elseif(CMAKE_HOST_WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(FoundationIO ws2_32 bcrypt)
endif(CMAKE_HOST_UNIX)

find_package(Git)

include(CMakePrintHelpers)

if(GIT_FOUND)
    file(READ "../Library/include/Private/FoundationIO.h" FoundationIO_Header)

    string(REGEX MATCH "#define FoundationIO_Version_Major [0-9]+" _ "${FoundationIO_Header}")
    string(REGEX MATCH "[0-9]+" VERSION_MAJOR "${_}")

    string(REGEX MATCH "#define FoundationIO_Version_Minor [0-9]+" _ "${FoundationIO_Header}")
    string(REGEX MATCH "[0-9]+" VERSION_MINOR "${_}")

    string(REGEX MATCH "#define FoundationIO_Version_Patch [0-9]+" _ "${FoundationIO_Header}")
    string(REGEX MATCH "[0-9]+" VERSION_PATCH "${_}")


    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify master WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE GIT_COMMIT_ID RESULTS_VARIABLE STDOUT ERROR_VARIABLE STDERR)
    string(REGEX REPLACE "\n$" "" GIT_COMMIT_ID_STRIPPED "${GIT_COMMIT_ID}")
    string(TOUPPER "${GIT_COMMIT_ID_STRIPPED}" GIT_COMMIT_ID_STRIPPED_CAPS)

    set(FoundationIO_VERSION_STRING "")
    string(APPEND FoundationIO_VERSION_STRING "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}" ":" "${GIT_COMMIT_ID_STRIPPED_CAPS}")

    set(FoundationIO_VERSION_RESOURCE "")
    string(APPEND FoundationIO_VERSION_RESOURCE "${VERSION_MAJOR}" "," "${VERSION_MINOR}" "," "${VERSION_PATCH}" "," "${GIT_COMMIT_ID_STRIPPED_CAPS}")
endif(GIT_FOUND)

configure_file("${FoundationIO_Resource_Dir}/FoundationIO.pc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/FoundationIO.pc")
configure_file("${FoundationIO_Resource_Dir}/FoundationIO.rc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/FoundationIO.rc")

option(BUILD_AS_SHARED_LIBRARY "Should FoundationIO be compiled as a dynamic library (.dylib/.dll/.so)?" ON)

if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(FoundationIO SHARED ${FOUNDATIONIO_SOURCE})
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(FoundationIO STATIC ${FOUNDATIONIO_SOURCE})
endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

set_target_properties(FoundationIO PROPERTIES PREFIX "")

target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include")
target_include_directories(FoundationIO PUBLIC "${FoundationIO_Library_Dir}/include/TextIO")

if(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_WIN32)
    set(CMAKE_INSTALL_PREFIX "%ProgramFiles%\FoundationIO")
endif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)

install(TARGETS FoundationIO                  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES  ${FOUNDATIONIO_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES  ${TEXTIO_PUBLIC_HEADERS}       DESTINATION ${CMAKE_INSTALL_PREFIX}/include/TextIO)
install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/FoundationIO.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

if(BUILD_TESTING)
    enable_testing()

    add_executable(Test_BufferIO                    "${FoundationIO_Test_Dir}/Test_BufferIO.c")
    add_executable(Test_CryptographyIO              "${FoundationIO_Test_Dir}/Test_CryptographyIO.c")
    add_executable(Test_FileIO                      "${FoundationIO_Test_Dir}/Test_FileIO.c")
    add_executable(Test_MathIO                      "${FoundationIO_Test_Dir}/Test_MathIO.c")
    add_executable(Test_NetworkIO                   "${FoundationIO_Test_Dir}/Test_NetworkIO.c")
    add_executable(Test_CommandLineIO               "${FoundationIO_Test_Dir}/TextIO/Test_CommandLineIO.c")
    add_executable(Test_FormatIO                    "${FoundationIO_Test_Dir}/TextIO/Test_FormatIO.c")
    add_executable(Test_StringIO                    "${FoundationIO_Test_Dir}/TextIO/Test_StringIO.c")

    target_link_libraries(Test_BufferIO             FoundationIO)
    target_link_libraries(Test_CryptographyIO       FoundationIO)
    target_link_libraries(Test_FileIO               FoundationIO)
    target_link_libraries(Test_MathIO               FoundationIO)
    target_link_libraries(Test_NetworkIO            FoundationIO)
    target_link_libraries(Test_CommandLineIO        FoundationIO)
    target_link_libraries(Test_FormatIO             FoundationIO)
    target_link_libraries(Test_StringIO             FoundationIO)

    add_test(Test_BufferIO                          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_BufferIO")
    add_test(Test_CryptographyIO                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_CryptographyIO")
    add_test(Test_FileIO                            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_FileIO")
    add_test(Test_MathIO                            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_MathIO")
    add_test(Test_NetworkIO                         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_NetworkIO")
    add_test(Test_CommandLineIO                     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_CommandLineIO")
    add_test(Test_FormatIO                          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_FormatIO")
    add_test(Test_StringIO                          "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Test_StringIO")

    install(TARGETS Test_BufferIO       DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_CommandLineIO  DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_CryptographyIO DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_FormatIO       DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_FileIO         DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_MathIO         DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_NetworkIO      DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS Test_StringIO       DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

    add_custom_command(TARGET FoundationIO POST_BUILD COMMAND ctest ARGS --output-on-failure WORKING_DIRECTORY ${CMAKE_OUTPUT_DIRECTORY})

endif(BUILD_TESTING)

if(${CMAKE_C_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fshort-enums \
-funsigned-char \
-std=c18 \
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
-Wsuggest-attribute=const \
-Wenum-conversion \
-v \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
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
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
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

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
")

elseif(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

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
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
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
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
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
")

set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
")

elseif(${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")

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

endif(${CMAKE_C_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
