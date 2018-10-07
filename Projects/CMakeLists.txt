cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(FoundationIO)

if(NOT FoundationIO_VERSION)
SET(FoundationIO_VERSION_MAJOR 0)
SET(FoundationIO_VERSION_MINOR 9)
SET(FoundationIO_VERSION_PATCH 1)
SET(FoundationIO_VERSION ${FoundationIO_VERSION_MAJOR}.${FoundationIO_VERSION_MINOR}.${FoundationIO_VERSION_PATCH})
endif(NOT FoundationIO_VERSION)

if   (${CMAKE_SYSTEM_NAME} MATCHES "UNIX")
add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)
else (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(-D_LIB)
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
endif

set(libFoundationIO_Dir "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${libFoundationIO_Dir}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${libFoundationIO_Dir}/../BUILD/${ARCHITECTURE}/${BUILDTYPE}")

set(libFoundationIO_Headers 
	"${libFoundationIO_Dir}/include/BitIO.h"
	"${libFoundationIO_Dir}/include/CommandLineIO.h"
	"${libFoundationIO_Dir}/include/ContainerIO.h"
	"${libFoundationIO_Dir}/include/Log.h"
	"${libFoundationIO_Dir}/include/Math.h"
	"${libFoundationIO_Dir}/include/StringIO.h"
)

set(libFoundationIO_Sources
	"${libFoundationIO_Dir}/src/BitIO.c"
	"${libFoundationIO_Dir}/src/CommandLineIO.c"
	"${libFoundationIO_Dir}/src/ContainerIO.c"
	"${libFoundationIO_Dir}/src/Log.c"
	"${libFoundationIO_Dir}/src/Math.c"
	"${libFoundationIO_Dir}/src/StringIO.c"
)

if (UNIX)
set(libMATH -lm)
else (WIN32)
set(libMATH)
endif

add_library(${project} [static|shared] ${libFoundationIO_Sources} ${libFoundationIO_Headers})



if (CMAKE_C_COMPILER_ID == AppleClang || CMAKE_C_COMPILER_ID == Clang || CMAKE_C_COMPILER_ID == GNU)
	set(LINK_FLAGS "${LINK_FLAGS}" -flto)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" -std=c11 -ferror-limit=1024 -Weverything -fcolor-diagnostics -fconst-strings -fshort-enums -vectorize-loops -funroll-loops -funsigned-char -Wno-c99-compat -fstrict-enums)

	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O1")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -Ofast")
	set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os")
endif(CMAKE_C_COMPILER_ID == AppleClang || CMAKE_C_COMPILER_ID == Clang || CMAKE_C_COMPILER_ID == GNU)

if (CMAKE_C_COMPILER_ID == MSVC)
	set(LINK_FLAGS "${LINK_FLAGS}" /GL)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" /std:c++17 /GF /bigobj /Qpar /J /JMC /MP /Ox /Ob2 /Os /GT /MT /openmp /FAu /LTCG /p:CharacterSet=Unicode /p:UseOfMfc=false /p:WarningLevel=Level3 /p:PrecompiledHeader=NotUsing /p:FunctionLevelLinking=true /p:RuntimeLibrary=MultiThreaded)

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /p:UseDebugLibraries=true")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /p:UseDebugLibraries=false")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /p:UseDebugLibraries=true")
	set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /p:UseDebugLibraries=false")
endif(CMAKE_C_COMPILER_ID == MSVC)
