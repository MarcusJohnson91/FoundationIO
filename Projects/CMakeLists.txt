cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(FoundationIO)

add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)

set(libFoundationIO_Dir "${CMAKE_CURRENT_SOURCE_DIR}")

set(ARCHIVE_OUTPUT_DIRECTORY "${libFoundationIO_Dir}/../BUILD/${ARCHITECTURE}/${BUILDTYPE})

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
	set(LINK_FLAGS "${LINK_FLAGS}" -flto)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" -std=c11 -ferror-limit=1024 -Weverything -fcolor-diagnostics -fconst-strings -fshort-enums -vectorize-loops -funroll-loops -funsigned-char -Wno-c99-compat -fstrict-enums)

	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O1")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -Ofast")
	set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os")
endif(CMAKE_C_COMPILER_ID == MSVC)
