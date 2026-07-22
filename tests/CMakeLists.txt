# Create unit test target

add_executable( ${DEV_UNIT_TEST_TARGET}

    "${DEV_UNIT_TEST_SOURCES}/main.cpp"
    
    "${DEV_UNIT_TEST_SOURCES}/adaptor/adapt.cpp"
    "${DEV_UNIT_TEST_SOURCES}/adaptor/chunkable.cpp"
    "${DEV_UNIT_TEST_SOURCES}/adaptor/divisible.cpp"
    
    "${DEV_UNIT_TEST_SOURCES}/concept/sizable_range.cpp"
    "${DEV_UNIT_TEST_SOURCES}/concept/tbb_compatible_range.cpp"
    
    "${DEV_UNIT_TEST_SOURCES}/utility/automatic_grain.cpp"
    "${DEV_UNIT_TEST_SOURCES}/utility/current_concurrency.cpp"
    "${DEV_UNIT_TEST_SOURCES}/utility/explicit_size.cpp"
    "${DEV_UNIT_TEST_SOURCES}/utility/invoke_with_concurrency.cpp"
)

target_include_directories( ${DEV_UNIT_TEST_TARGET} PRIVATE ${DEV_UNIT_TEST_HEADERS}     )
target_compile_features   ( ${DEV_UNIT_TEST_TARGET} PRIVATE ${DEV_UNIT_TEST_OPTIONS}     )
target_compile_options    ( ${DEV_UNIT_TEST_TARGET} PRIVATE ${UNIT_TEST_COMPILE_OPTIONS} )
target_link_options       ( ${DEV_UNIT_TEST_TARGET} PRIVATE ${UNIT_TEST_LINKAGE_OPTIONS} )


# Fetch unit test dependencies

CPMAddPackage(
    URI  "gh:doctest/doctest#v2.5.3"
    NAME "doctest"
)

target_link_libraries( ${DEV_UNIT_TEST_TARGET} PRIVATE ${DEV_INTERFACE_TARGET} doctest::doctest )


# Copy shared libraries to executable directory (necessary to find TBB DLLs when compiling for Windows)

add_custom_command( TARGET ${DEV_UNIT_TEST_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:TBB::tbb>
    $<TARGET_FILE_DIR:${DEV_UNIT_TEST_TARGET}>
)


# Register tests

include( "${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake" )

doctest_discover_tests( ${DEV_UNIT_TEST_TARGET} )
