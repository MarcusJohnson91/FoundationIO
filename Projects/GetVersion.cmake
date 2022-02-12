cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

find_package(Git)

if(GIT_FOUND)
function(GetVersionString VERSION_FILE OUTPUT_VERSION_STRING)
    file(READ ${VERSION_FILE} Version_Header)
    set(VERSION_LINE "")
    string(REGEX MATCH " @version         ([0-9]+)/.([0-9]+)/.([0-9]+)$" VERSION_LINE ${Version_Header})
    set("Version_Major" ${CMAKE_MATCH_2})
    set("Version_Minor" ${CMAKE_MATCH_3})
    set("Version_Patch" ${CMAKE_MATCH_4})

    if(IS_DIRECTORY ".git")
    set(GIT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    else
    file(READ ".git" GIT_FILE)
    set(GIT_ROOT_PATH "")
    string(REPLACE "gitdir: " "" GIT_ROOT_PATH ${GIT_FILE})
    endif(IS_DIRECTORY ".git")

    cmake_path(GET ${VERSION_FILE} ROOT_DIRECTORY ${GIT_ROOT_PATH})

    if(IS_DIRECTORY ".git")
        execute_process(COMMAND git ls-files -s ${VERSION_FILE} WORKING_DIRECTORY ${GIT_ROOT_PATH} OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    else
        execute_process(COMMAND git describe --abbrev=0 --always WORKING_DIRECTORY ${GIT_ROOT_PATH} OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif(IS_DIRECTORY ".git")

    string(TOUPPER ${GIT_VERSION} Version_CommitID)
    string(CONCAT "VERSION_STRING" "${Version_Major}" "." "${Version_Minor}" "." "${Version_Patch}" ":" "${Version_CommitID}")
    set("OUTPUT_VERSION_STRING" VERSION_STRING PARENT_SCOPE)
endfunction()
endif(GIT_FOUND)
