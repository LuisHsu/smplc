include(ExternalProject)
ExternalProject_add(googletest
    PREFIX ${PROJECT_SOURCE_DIR}/external/googletest
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/googletest
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CONFIGURE_COMMAND cmake -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> <SOURCE_DIR>
)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest/lib)
include(GoogleTest)