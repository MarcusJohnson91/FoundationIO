cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

find_package(Git)

if(GIT_FOUND)
function(GetVersionString VERSION_FILE OUTPUT_VERSION_STRING)
    file(READ ${VERSION_FILE} Version_Header)
    set(VERSION_LINE "")
    string(REGEX MATCH " @version         ([0-9]+)\.([0-9]+)\.([0-9]+)$" VERSION_LINE ${Version_Header})
    set("Version_Major" ${CMAKE_MATCH_2})
    set("Version_Minor" ${CMAKE_MATCH_3})
    set("Version_Patch" ${CMAKE_MATCH_4})

    # cd /path/to/parent/repo && git ls-files -s yourSubmodule
    # How do we detect if we're a submodule though?
    # FoundationIO is always Dependencies/FoundationIO; LIVC is Library/External/LIVC though
    # Go up one folder and check if it's Dependencies or External?
    # Could just create a variable in the owning repo

    # like FoundationIO's cmakelist would say it's the original, and OVIA could set it that it's a submodule

    set(VERSION_PATH "")

    if(IsSubmodule)
        cmake_path(GET ${VERSION_FILE} ROOT_DIRECTORY VERSION_PATH) # I don't think this will work with submodules
        execute_process(COMMAND git describe --abbrev=0 --always WORKING_DIRECTORY ${VERSION_PATH} OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    elseif(NOT IsSubmodule)
        cmake_path(GET ${VERSION_FILE} ROOT_DIRECTORY VERSION_PATH) # I don't think this will work with submodules
        execute_process(COMMAND git ls-files -s ${VERSION_FILE} WORKING_DIRECTORY ${VERSION_PATH} OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif(IsSubmodule)

    string(TOUPPER ${GIT_VERSION} Version_CommitID)
    string(CONCAT "VERSION_STRING" "${Version_Major}" "." "${Version_Minor}" "." "${Version_Patch}" ":" "${Version_CommitID}")
    set("OUTPUT_VERSION_STRING" "${VERSION_STRING}" PARENT_SCOPE)
endfunction()
endif(GIT_FOUND)
