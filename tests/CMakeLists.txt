# Create test target
add_executable(cfr_ut
    "source/main.cpp"
)

target_compile_features   (cfr_ut PRIVATE ${TMC_COMPILE_FEATURES}              )
target_compile_options    (cfr_ut PRIVATE ${TMC_TEST_COMPILER_FLAGS}           )
target_link_options       (cfr_ut PRIVATE ${TMC_TEST_LINKER_FLAGS}             )
target_include_directories(cfr_ut PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link the main library
target_link_libraries(cfr_ut cfr)

# Register for CTest
add_test(
    NAME cfr_ut
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cfr_ut --no-intro --no-path-filenames --force-colors
)