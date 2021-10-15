cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

set(DetectArchitectureInC "
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error TargetArchitecture x86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error TargetArchitecture x86
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
    #error TargetArchitecture IA64
#elif defined(__arm__) || defined(__TARGET_ARCH_ARM)
    #if defined(__ARM_ARCH_8__) || defined(__ARM_ARCH_8A__) || defined(__ARM_ARCH_8R__) || defined(__ARM_ARCH_8M__) || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 8)
        #error TargetArchitecture ARMv8
    #if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
        #error TargetArchitecture ARMv7
    #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6M__) || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
        #error TargetArchitecture ARMv6
    #elif defined(__ARM_ARCH_5TEJ__) || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
        #error TargetArchitecture ARMv5
    #else
        #error TargetArchitecture ARM
    #endif
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC) || defined(_M_MPPC) || defined(_M_PPC)
    #if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
        #error TargetArchitecture PPC64
    #else
        #error TargetArchitecture PPC
    #endif
#elif defined(_MIPS_ARCH) || defined(__mips) || defined(__mips__) || defined(_mips)
    #if (_MIPS_SZPTR == 64)
        #error TargetArchitecture MIPS64
    #elif (_MIPS_SZPTR == 32)
        #error TargetArchitecture MIPS32
    #endif
#elif defined(__RISCV_ARCH) || defined(__riscv) || defined(__mips__) || defined(_mips)
    #if (__POINTER_WIDTH__ == 64)
        #error TargetArchitecture RISCV64
    #elif (__POINTER_WIDTH__ == 32)
        #error TargetArchitecture RISCV32
    #endif
#else
    #error TargetArchitecture Unknown
#endif
")

function(FindTargetArchitecture Result)
    if(APPLE AND CMAKE_OSX_ARCHITECTURES)
        foreach(Apple_Arch ${CMAKE_OSX_ARCHITECTURES})
            if("${Apple_Arch}" STREQUAL "x86")
                set(osx_arch_i386 TRUE)
            elseif("${Apple_Arch}" STREQUAL "x86_64")
                set(osx_arch_x86_64 TRUE)
            elseif("${Apple_Arch}" STREQUAL "ARM64")
                set(osx_arch_arm64 TRUE)
            elseif("${Apple_Arch}" STREQUAL "ARM")
                set(osx_arch_arm TRUE)
            else()
                message(FATAL_ERROR "Invalid OS X arch name: ${Apple_Arch}")
            endif()
        endforeach()

        # Now add all the architectures in our normalized order
        if(osx_arch_x86_64)
            list(APPEND ARCH x86_64)
        endif()

        if(osx_arch_x86)
            list(APPEND ARCH x86)
        endif()

        if(osx_arch_ARM64)
            list(APPEND ARCH ARM64)
        endif()

        if(osx_arch_ARM)
            list(APPEND ARCH ARM)
        endif()
    else()
        file(WRITE "${CMAKE_BINARY_DIR}/DetectArch.c" "${DetectArchitectureInC}")

        enable_language(C)

        try_run(
            run_result_unused
            compile_result_unused
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_BINARY_DIR}/DetectArch.c"
            COMPILE_OUTPUT_VARIABLE ARCH
            CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        )

        string(REGEX MATCH "TargetArchitecture ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

        string(REPLACE "TargetArchitecture " "" ARCH "${ARCH}")

        if (NOT ARCH)
            set(ARCH Unknown)
        endif()

        file(REMOVE "${CMAKE_BINARY_DIR}/DetectArch.c")
    endif()

    set(${Result} "${ARCH}" PARENT_SCOPE)
endfunction()
