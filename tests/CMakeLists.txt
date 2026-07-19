# Create target

add_executable( "${PROJECT_NAME}_ut"
    "source/main.cpp"
    
    "source/adaptor/chunkable.cpp"
    "source/adaptor/divisible.cpp"
    
    "source/utility/automatic_grain.cpp"
    "source/utility/explicit_size.cpp"
)

target_compile_features   ( "${PROJECT_NAME}_ut" PRIVATE "${CFR_COMPILE_FEATURES}"            )
target_compile_options    ( "${PROJECT_NAME}_ut" PRIVATE "${CFR_TEST_COMPILE_OPTIONS}"        )
target_link_options       ( "${PROJECT_NAME}_ut" PRIVATE "${CFR_TEST_LINK_OPTIONS}"           )
target_include_directories( "${PROJECT_NAME}_ut" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link dependencies

target_link_libraries( "${PROJECT_NAME}_ut"
    PRIVATE
    "${PROJECT_NAME}::lib"
    "doctest::doctest"
)

set_target_properties( "${PROJECT_NAME}_ut" "tbb"
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # `.exe` / `.dll`
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" # `.so`  / `.dylib`
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # `.lib` / `.a`
)

# Note: Without explicitly telling TBB to put it's DLLs into the same directory as executable
#       we would be unable to locate them when running on Windows without TBB package install

# Register CTest

include( "${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake" )

doctest_discover_tests( "${PROJECT_NAME}_ut" )
