# Parser regression tests
file(GLOB_RECURSE test_files 
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    Parser/*.*smpl
)
foreach(test_file IN LISTS test_files)
    get_filename_component(test_title_path ${test_file} DIRECTORY)
    get_filename_component(test_name ${test_file} NAME_WE)
    string(REGEX REPLACE / _ test_title ${test_title_path})
    add_test(NAME "regression_${test_title}.${test_name}"
        COMMAND ${CMAKE_BINARY_DIR}/exec/smplc ${CMAKE_CURRENT_LIST_DIR}/${test_file}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
    if(${test_file} MATCHES "\.invalid")
        set_tests_properties("regression_${test_title}.${test_name}"
            PROPERTIES WILL_FAIL true
        )
    endif()
endforeach(test_file IN LISTS test_files)
