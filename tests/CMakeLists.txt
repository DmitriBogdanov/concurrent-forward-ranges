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

target_link_libraries( "${PROJECT_NAME}_ut" PRIVATE "${PROJECT_NAME}::lib" )

# Register for CTest

add_test(
    NAME    "${PROJECT_NAME}_ut"
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/cfr_ut" --no-intro --no-path-filenames --force-colors --duration
)