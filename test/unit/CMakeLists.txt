# Introduce GoogleTest
include(ExternalProject)
ExternalProject_add(googletest
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/googletest
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CONFIGURE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> <SOURCE_DIR>
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest/lib)
include(GoogleTest)

# Function for add unit tests
function(add_unittest test_name)
    add_executable(${ARGV})
    add_dependencies(${ARGV0}
        googletest
    )
    target_link_libraries(${ARGV0}
        smplc
        gtest
        gtest_main
        pthread
    )
    gtest_discover_tests(${ARGV0})
endfunction(add_unittest)

# Unit tests
add_unittest(test_Source
    test_Source.cpp
)
add_unittest(test_Parser_Letter
    test_Parser_Letter.cpp
)
add_unittest(test_Parser_Digit
    test_Parser_Digit.cpp
)
add_unittest(test_Parser_RelOp
    test_Parser_RelOp.cpp
)