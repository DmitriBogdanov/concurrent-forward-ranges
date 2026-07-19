cmake_minimum_required( VERSION 3.21.0...3.22.1 )


# Setup project

set( DEV_PROJECT "cfr"   )
set( DEV_VERSION "0.1.0" )

set( DEV_DESCRIPTION "Concurrent range algorithms for C++23"                       )
set( DEV_REPOSITORY  "https://github.com/DmitriBogdanov/concurrent-forward-ranges" )

project( ${DEV_PROJECT}
    VERSION      ${DEV_VERSION}
    DESCRIPTION  ${DEV_DESCRIPTION}
    HOMEPAGE_URL ${DEV_REPOSITORY}
)

set( DEV_INTERFACE_TARGET "cfr_interface" )
set( DEV_UNIT_TEST_TARGET "cfr_unit_test" )
set( DEV_BENCHMARK_TARGET "cfr_benchmark" )

set( DEV_INTERFACE_ALIAS "cfr::cfr" )

set( DEV_INTERFACE_HEADERS "${PROJECT_SOURCE_DIR}/include/"       )
set( DEV_INTERFACE_SOURCES "${PROJECT_SOURCE_DIR}/source/"        )
set( DEV_INTERFACE_OPTIONS "cxx_std_23"                           )

set( DEV_UNIT_TEST_HEADERS "${PROJECT_SOURCE_DIR}/tests/include/" )
set( DEV_UNIT_TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/source/"  )
set( DEV_UNIT_TEST_OPTIONS "cxx_std_23"                           )


# Create interface target

add_library( ${DEV_INTERFACE_TARGET} INTERFACE )

target_include_directories( ${DEV_INTERFACE_TARGET} INTERFACE ${DEV_INTERFACE_HEADERS} )
target_compile_features   ( ${DEV_INTERFACE_TARGET} INTERFACE ${DEV_INTERFACE_OPTIONS} )


# Fetch interface dependencies

include( "${PROJECT_SOURCE_DIR}/cmake/CPM.cmake" )

CPMAddPackage(
    URI     "gh:uxlfoundation/oneTBB#v2023.1.0"
    NAME    "TBB"
    OPTIONS "TBB_TEST OFF" "TBB_EXAMPLES OFF"
)

target_link_libraries( ${DEV_INTERFACE_TARGET} INTERFACE TBB::tbb )


# Create interface alias

add_library( ${DEV_INTERFACE_ALIAS} ALIAS ${DEV_INTERFACE_TARGET} )


# Project is top-level => build developer targets

if( "${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" )
    
    include( CTest )
    
    enable_testing()
    
    add_subdirectory( "tests" )
    
endif()
