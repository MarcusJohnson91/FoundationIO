cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

find_package(Git)

if(GIT_FOUND)
# VersionString is the output variable, VersionFilesFolder is the path where Version.h and CommitID can be found.
function(GetVersionString VersionString VersionFilesFolder)
    string(CONCAT VersionHeaderPath ${VersionFilesFolder} "/Version.h")
    file(READ ${VersionHeaderPath} Version_Header)

    string(REGEX MATCH "#define Version_Major ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Minor ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Patch ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_PATCH ${CMAKE_MATCH_1})

    execute_process(COMMAND ${GIT_EXECUTABLE} symbolic-ref -q HEAD WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE "GIT_BRANCH_PATH" RESULTS_VARIABLE STDOUT ERROR_VARIABLE STDERR)

    string(REGEX REPLACE "\n$" "" "GIT_BRANCH_PATH_STRIPPED" "${GIT_BRANCH_PATH}")

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify ${GIT_BRANCH_PATH_STRIPPED} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE "GIT_COMMIT_ID" RESULTS_VARIABLE STDOUT ERROR_VARIABLE STDERR)

    string(REGEX REPLACE "\n$" "" "GIT_COMMIT_ID_STRIPPED" "${GIT_COMMIT_ID}")

    string(TOUPPER "${GIT_COMMIT_ID_STRIPPED}" "GIT_COMMIT_ID_STRIPPED_CAPS")

    set(VERSION_STRING "")
    string(CONCAT VERSION_STRING "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}" ":" "${GIT_COMMIT_ID_STRIPPED_CAPS}")

    string(APPEND "CommitID_Header" "#pragma once\n\n#define Version_CommitID " "${GIT_COMMIT_ID_STRIPPED_CAPS}")

    string(CONCAT VersionHeaderPath ${VersionFilesFolder} "/CommitID.h")
    file(WRITE ${VersionHeaderPath} "${CommitID_Header}")

    set(${VersionString} "${VERSION_STRING}" PARENT_SCOPE)
endfunction()
endif(GIT_FOUND)
