macro(RegisterTest Path Name LibraryName)
    string(CONCAT Test_Name "Test_${Name}")
    add_executable(${Test_Name} "${Path}/${Test_Name}.c")
    target_link_libraries(${Test_Name} ${LibraryName})
    set(${LibraryName}_TEST_TARGETS ${${LibraryName}_TEST_TARGETS} ${Test_Name})
    add_test(${Test_Name} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Test_Name}")
    set_tests_properties(${Test_Name} PROPERTIES TIMEOUT 15)

    add_custom_command(TARGET ${LibraryName}_Tests COMMENT "Run ${LibraryName} tests" POST_BUILD COMMAND ctest ARGS --output-on-failure WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endmacro(RegisterTest)

#set(LibraryTests ${LibraryName}_Tests)
