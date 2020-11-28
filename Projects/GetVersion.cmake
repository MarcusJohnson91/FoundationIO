cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

find_package(Git)

if(GIT_FOUND)
function(GetVersionString VersionString VersionFilesFolder)

get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

string(CONCAT VersionHeaderPath "${PARENT_DIR}${VersionFilesFolder}/Version.h")
file(READ ${VersionHeaderPath} Version_Header)

if(CMAKE_HOST_UNIX)
    #add_custom_target(GitHash ALL COMMAND sh ./GetGitHash.sh)
    execute_process(COMMAND sh "${CMAKE_SOURCE_DIR}/GetGitHash.sh")
else(CMAKE_HOST_WIN32)
    #add_custom_target(GitHash ALL COMMAND cmd.exe "${CMAKE_SOURCE_DIR}/GetGitHash.cmd")
    execute_process(COMMAND cmd.exe "./GetGitHash.cmd")
endif(CMAKE_HOST_UNIX)

string(CONCAT CommitIDHeaderPath "${PARENT_DIR}${VersionFilesFolder}/CommitID.h")
file(READ ${CommitIDHeaderPath} CommitID_Header)

    string(REGEX MATCH "#define Version_Major ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Minor ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Patch ([0-9]+)" MATCH_STRING ${Version_Header})
    set(VERSION_PATCH ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_CommitID ([0-9]+)" MATCH_STRING ${CommitID_Header})
    set(VERSION_COMMITID ${CMAKE_MATCH_1})

    string(REGEX REPLACE "\n$" "" "GIT_COMMIT_ID_STRIPPED" "${GIT_COMMIT_ID}")

    string(TOUPPER "${GIT_COMMIT_ID_STRIPPED}" "GIT_COMMIT_ID_STRIPPED_CAPS")

    set(VERSION_STRING "")
    string(CONCAT VERSION_STRING "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}" ":" "${GIT_COMMIT_ID_STRIPPED_CAPS}")

    set(${VersionString} "${VERSION_STRING}" PARENT_SCOPE)
endfunction()
endif(GIT_FOUND)
