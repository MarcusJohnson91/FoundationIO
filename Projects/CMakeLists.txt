cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(FoundationIO)

if(NOT FoundationIO_VERSION)
set(FoundationIO_VERSION_MAJOR 0)
set(FoundationIO_VERSION_MINOR 9)
set(FoundationIO_VERSION_PATCH 1)
set(FoundationIO_VERSION ${FoundationIO_VERSION_MAJOR}.${FoundationIO_VERSION_MINOR}.${FoundationIO_VERSION_PATCH})
endif(NOT FoundationIO_VERSION)

if   (${CMAKE_SYSTEM_NAME} MATCHES "UNIX" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(libMATH -lm)
add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(${CMAKE_SYSTEM_NAME} MATCHES "UNIX" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(libFoundationIO_Dir "${CMAKE_CURRENT_SOURCE_DIR}/../libFoundationIO")

set(Test_Dir "${CMAKE_CURRENT_SOURCE_DIR}/../Tests")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")
set(RUNTIME_OUTPUT_DIRECTORY       "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")

set(libFoundationIO_PublicHeaders
"${libFoundationIO_Dir}/include/BitIO.h"
"${libFoundationIO_Dir}/include/CommandLineIO.h"
"${libFoundationIO_Dir}/include/ContainerIO.h"
"${libFoundationIO_Dir}/include/CryptographyIO.h"
"${libFoundationIO_Dir}/include/LocalizationIO.h"
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
"${libFoundationIO_Dir}/src/CryptographyIO.c"
"${libFoundationIO_Dir}/src/LocalizationIO.c"
"${libFoundationIO_Dir}/src/Log.c"
"${libFoundationIO_Dir}/src/Macros.c"
"${libFoundationIO_Dir}/src/Math.c"
"${libFoundationIO_Dir}/src/StringIO.c"
"${libFoundationIO_Dir}/src/TestIO.c"
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "${MinSizeRel}")
endif(NOT CMAKE_BUILD_TYPE)

add_library(FoundationIO ${libFoundationIO_Sources} ${libFoundationIO_PublicHeaders})
set_target_properties(FoundationIO PROPERTIES PREFIX "")

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



if (CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_C_COMPILER_ID MATCHES "Clang")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fPIE \
-fshort-enums \
-funroll-loops \
-funsigned-char \
-std=c17 \
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
-Wno-varargs \
-v \
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
-Oz \
")

set(LINK_FLAGS ${LINK_FLAGS} "-flto=full")

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

elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fPIE \
-fshort-enums \
-funroll-loops \
-funsigned-char \
-std=c17 \
-Wall \
-Wextra \
-Wpedantic \
-Wno-ignored-attributes \
-Wno-padded \
-Wno-pointer-sign \
-Wno-sign-conversion \
-Wno-varargs \
-Wno-discarded-qualifier \
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

set(LINK_FLAGS ${LINK_FLAGS} "-flto=full")

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

elseif (MAKE_C_COMPILER_ID MATCHES "MSVC")

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

endif (CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
