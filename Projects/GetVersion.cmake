cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0066 NEW)

find_package(Git)

if(GIT_FOUND)
function(GetVersionString VERSION_DIRECTORY VERSION_STRING)
    get_filename_component(PARENT_DIR ${VERSION_DIRECTORY} DIRECTORY)

    string(CONCAT VersionHeaderPath "${PARENT_DIR}/Version.h")
    file(READ ${VersionHeaderPath} Version_Header)

    string(CONCAT CommitIDHeaderPath "${PARENT_DIR}/CommitID.h")

    if(CMAKE_HOST_UNIX)
        execute_process(COMMAND sh "${BASE_DIRECTORY}/Projects/GetGitHash.sh")
    else(CMAKE_HOST_WIN32)
        configure_file("${CMAKE_SOURCE_DIR}/GetGitHash.cmd.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Scripts/GetGitHash.cmd")
        execute_process(COMMAND cmd.exe "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Scripts/GetGitHash.cmd")
        #file(REMOVE_RECURSE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Scripts/GetGitHash.cmd")
    endif(CMAKE_HOST_UNIX)

    file(READ ${CommitIDHeaderPath} CommitID_Header)
    
    string(REGEX MATCH "#define Version_Major ([0-9]+)" MATCH_STRING ${Version_Header})
    set("VERSION_MAJOR" ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Minor ([0-9]+)" MATCH_STRING ${Version_Header})
    set("VERSION_MINOR" ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_Patch ([0-9]+)" MATCH_STRING ${Version_Header})
    set("VERSION_PATCH" ${CMAKE_MATCH_1})
    string(REGEX MATCH "#define Version_CommitID ([0-9a-fA-F]+)" MATCH_STRING ${CommitID_Header})
    set("VERSION_COMMITID" ${CMAKE_MATCH_1})

    string(REGEX REPLACE "\n$" "" "GIT_COMMIT_ID_STRIPPED" "${VERSION_COMMITID}")

    string(TOUPPER "${GIT_COMMIT_ID_STRIPPED}" "GIT_COMMIT_ID_STRIPPED_CAPS")

    string(CONCAT "VERSION_STRING" "${VERSION_MAJOR}" "." "${VERSION_MINOR}" "." "${VERSION_PATCH}" ":" "${GIT_COMMIT_ID_STRIPPED_CAPS}")

    set("VERSION_STRING" "${VERSION_STRING}" PARENT_SCOPE)
    #set(${Result} "${ARCH}" PARENT_SCOPE)
    #return(${VERSION_STRING})
endfunction()
endif(GIT_FOUND)
