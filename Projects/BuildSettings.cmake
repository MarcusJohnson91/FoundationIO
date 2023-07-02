if(CMAKE_HOST_UNIX)
    add_definitions(-D_LARGEFILE_SOURCE)
    add_definitions(-D_LARGEFILE64_SOURCE)
    add_definitions(-D_FILE_OFFSET_BITS=64)
    add_definitions(-D__STDC_WANT_LIB_EXT1__)
    add_definitions(-D__TIMESIZE=64)
elseif(CMAKE_HOST_WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(CMAKE_HOST_UNIX)

if($<C_COMPILER_ID:AppleClang,ARMClang,XLClang,Clang>)

set(CMAKE_EXE_LINKER_FLAGS_INIT "-fsanitize=address,undefined -fno-omit-frame-pointer")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-mtune=x86-64-v2 \
-std=c2x \
-fPIC \
-ffat-lto-objects \
-Wall \
-Wextra \
-Weverything \
-Bsymbolic \
-Bsymbolic-functions \
-ferror-limit=1024 \
-foptimize-sibling-calls \
-funsigned-char \
-fvisibility-inlines-hidden \
-Wenum-conversion \
-Wno-assign-enum \
-Wno-c99-compat \
-Wno-ignored-attributes \
-Wno-incompatible-pointer-types-discards-qualifiers \
-Wno-padded \
-Wno-pointer-sign \
-Wno-reserved-id-macro \
-Wno-sign-conversion \
-Wpedantic \
-Wreserved-identifier \
-Wswitch-enum \
-Wno-unknown-pragmas \
-Rpass=loop-vectorize \
-Rpass-missed=loop-vectorize \
-Rpass-analysis=loop-vectorize \
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
-g3 \
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
-ffat-lto-objects \
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
-Wno-unknown-pragmas \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fno-omit-frame-pointer \
-fsanitize=address,undefined \
-fstack-check \
-DDEBUG=1 \
-g3 \
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
-g3 \
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
/wd4146 \
/wd4245 \
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

