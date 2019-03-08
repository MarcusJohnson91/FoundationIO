cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(FoundationIO)

if(NOT FoundationIO_VERSION)
set(FoundationIO_VERSION_MAJOR 0)
set(FoundationIO_VERSION_MINOR 9)
set(FoundationIO_VERSION_PATCH 1)
set(FoundationIO_VERSION ${FoundationIO_VERSION_MAJOR}.${FoundationIO_VERSION_MINOR}.${FoundationIO_VERSION_PATCH})
endif(NOT FoundationIO_VERSION)

if   (${CMAKE_SYSTEM_NAME} MATCHES "UNIX")
set(libMATH -lm)
add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)
else (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(-D_LIB)
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(${CMAKE_SYSTEM_NAME} MATCHES "UNIX")

set(libFoundationIO_Dir "${CMAKE_CURRENT_SOURCE_DIR}/../libFoundationIO")

set(Test_Dir "${CMAKE_CURRENT_SOURCE_DIR}/../Tests")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${libFoundationIO_Dir}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${libFoundationIO_Dir}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")

set(libFoundationIO_PublicHeaders
"${libFoundationIO_Dir}/include/BitIO.h"
"${libFoundationIO_Dir}/include/CommandLineIO.h"
"${libFoundationIO_Dir}/include/ContainerIO.h"
"${libFoundationIO_Dir}/include/Log.h"
"${libFoundationIO_Dir}/include/Macros.h"
"${libFoundationIO_Dir}/include/Math.h"
"${libFoundationIO_Dir}/include/StringIO.h"
"${libFoundationIO_Dir}/include/TestIO.h"
)

set(libFoundationIO_Sources
"${libFoundationIO_Dir}/src/BitIO.c"
"${libFoundationIO_Dir}/src/CommandLineIO.c"
"${libFoundationIO_Dir}/src/ContainerIO.c"
"${libFoundationIO_Dir}/src/Log.c"
"${libFoundationIO_Dir}/src/Macros.c"
"${libFoundationIO_Dir}/src/Math.c"
"${libFoundationIO_Dir}/src/StringIO.c"
"${libFoundationIO_Dir}/src/TestIO.c"
)

add_library(FoundationIO ${libFoundationIO_Sources} ${libFoundationIO_PublicHeaders})

add_executable(Test_BitIO "${Test_Dir}/Test_BitIO.c")
add_executable(Test_CommandLineIO "${Test_Dir}/Test_CommandLineIO.c")
add_executable(Test_ContainerIO "${Test_Dir}/Test_ContainerIO.c")
add_executable(Test_StringIO "${Test_Dir}/Test_StringIO.c")

target_link_libraries(Test_BitIO FoundationIO)
target_link_libraries(Test_CommandLineIO FoundationIO)
target_link_libraries(Test_ContainerIO FoundationIO)
target_link_libraries(Test_StringIO FoundationIO)

add_test(Test_BitIO CONFIGURATIONS Debug Release)
add_test(Test_CommandLineIO CONFIGURATIONS Debug Release)
add_test(Test_ContainerIO CONFIGURATIONS Debug Release)
add_test(Test_StringIO CONFIGURATIONS Debug Release)

if (CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
set(LINK_FLAGS " \
${LINK_FLAGS} \
/flto \
")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fcolor-diagnostics \
-ferror-limit=1024 \
-fshort-enums \
-fstrict-enums \
-funroll-loops \
-funsigned-char \
-std=c11 \
-Weverything \
-Wno-c99-compat \
-Wno-pointer-sign \
-Wno-padded \
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
-DDEBUG=1 \
-g \
-Ofast \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
-Os \
")

elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")

set(LINK_FLAGS " \
${LINK_FLAGS} \
/GL \
")

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
/LTCG \
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
endif(CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
