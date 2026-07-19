cmake_minimum_required( VERSION 3.21.0...3.22.1 )

project( cfr
    VERSION       0.1.0
    DESCRIPTION  "Concurrent range algorithms for C++23"
    HOMEPAGE_URL "https://github.com/DmitriBogdanov/concurrent-forward-ranges"
)

# Force all 

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin/" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib/" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib/" )

# Fetch library dependencies

include( cmake/CPM.cmake )

CPMAddPackage(
    URI     "gh:uxlfoundation/oneTBB#v2023.1.0"
    NAME    "TBB"
    OPTIONS "TBB_TEST OFF" "TBB_EXAMPLES OFF"
)

# Create library target

add_library( "${PROJECT_NAME}" INTERFACE )

target_include_directories( "${PROJECT_NAME}" INTERFACE "${PROJECT_SOURCE_DIR}/include" )
target_compile_features   ( "${PROJECT_NAME}" INTERFACE "${CFR_COMPILE_FEATURES}"       )
target_link_libraries     ( "${PROJECT_NAME}" INTERFACE "TBB::tbb"                      )

add_library( "${PROJECT_NAME}::lib" ALIAS "${PROJECT_NAME}" )

# Project is top-level => build tests, benchmarks and examples

if( "${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" )
    
    # Fetch developer dependencies
    
    CPMAddPackage(
        URI  "gh:doctest/doctest#v2.5.3"
        NAME "doctest"
    )

    # Enable testing
    
    include( CTest )
    
    enable_testing()
    
    # Create test target
    
    add_subdirectory( "tests" )
    
endif()
