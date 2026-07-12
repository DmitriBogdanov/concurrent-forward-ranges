cmake_minimum_required(VERSION 3.21.0...3.22.1)

project(
    CFR
    VERSION       0.21.0
    DESCRIPTION  "Comprehensive container library for C++23"
    HOMEPAGE_URL "https://github.com/DmitriBogdanov/too-many-containers"
)

# Find TBB
find_package(TBB)

# Create library target
add_library(cfr INTERFACE)

target_include_directories(cfr INTERFACE "${PROJECT_SOURCE_DIR}/include")
target_compile_features   (cfr INTERFACE cxx_std_23                     )
target_link_libraries     (cfr INTERFACE TBB::tbb                       )

add_library(concurrent-forward-ranges ALIAS cfr)

# Project is top-level => build benchmarks & tests
if("${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")

    include(CTest)
    enable_testing() # should be called before adding directories with tests
    
    add_subdirectory("tests")
    
endif()
