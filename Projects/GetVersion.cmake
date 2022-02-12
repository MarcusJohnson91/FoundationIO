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

    set(Submodule_Dir "")

    if(IS_DIRECTORY "../.git")
        set(GIT_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
        set(Submodule_Dir "../")
    else()
        file(READ "../.git" GIT_FILE)
        set(GIT_ROOT_PATH "" PARENT_SCOPE)
        string(REPLACE "gitdir: " "" GIT_ROOT_PATH ${GIT_FILE})
        string(CONCAT Submodule_Dir "${CMAKE_CURRENT_SOURCE_DIR}/../${GIT_ROOT_PATH}")
    endif(IS_DIRECTORY "../.git")

    set(GIT_CMD "git describe")
    set(GIT_ARGS "--abbrev=8 --always")
    set(GIT_VERSION "")
    execute_process(COMMAND   git describe --abbrev=8 --always
    WORKING_DIRECTORY ${Submodule_Dir}
    OUTPUT_VARIABLE GIT_VERSION)

    message("GIT_VERSION=${GIT_VERSION}")

    set(Version_CommitID "")
    string(TOUPPER ${GIT_VERSION} Version_CommitID)
    string(CONCAT VERSION_STRING "${Version_Major}" "." "${Version_Minor}" "." "${Version_Patch}" ":" "${Version_CommitID}")
    set(${OUTPUT_VERSION_STRING} VERSION_STRING PARENT_SCOPE)
endfunction()
endif(GIT_FOUND)
