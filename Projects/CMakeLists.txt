cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(libFoundationIO)

add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_LARGEFILE64_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)

set(ARCHIVE_OUTPUT_DIRECTORY "${libFoundationIORoot}/../BUILD/${ARCHITECTURE}/${BUILDTYPE})

set(libFoundationIO_Dir "${CMAKE_CURRENT_SOURCE_DIR}")

set(libFoundationIO_Headers 
	"${libFoundationIO_Dir}/include/Macros.h"
	"${libFoundationIO_Dir}/include/Math.h"
	"${libFoundationIO_Dir}/include/StringIO.h"
	"${libFoundationIO_Dir}/include/BitIO.h"
	"${libFoundationIO_Dir}/include/CommandLineIO.h"
	"${libFoundationIO_Dir}/include/GUUID.h"
	"${libFoundationIO_Dir}/include/Log.h"
)

set(libFoundationIO_Sources
	"${libFoundationIO_Dir}/src/Macros.c"
	"${libFoundationIO_Dir}/src/Math.c"
	"${libFoundationIO_Dir}/src/StringIO.c"
	"${libFoundationIO_Dir}/src/BitIO.c"
	"${libFoundationIO_Dir}/src/CommandLineIO.c"
	"${libFoundationIO_Dir}/src/GUUID.c"
	"${libFoundationIO_Dir}/src/Log.c"
)

if (UNIX)
set(libMATH -lm)
else (WIN32)
set(libMATH)
endif

add_library(${project} [static|shared] ${libFoundationIO_Sources} ${libFoundationIO_Headers})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS}" -flto)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" -std=c11 -ferror-limit=1024 -Weverything -fcolor-diagnostics -fconst-strings -fshort-enums -vectorize-loops -funroll-loops -funsigned-char -Wno-c99-compat -fstrict-enums)

	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O1")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
