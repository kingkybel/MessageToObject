add_executable(run_tests
    run_tests.cc
        generator_map_test.cc
        string_to_hash_test.cc
)

target_link_libraries(run_tests
    gtest
    gtest_main
)
add_test(NAME run_tests COMMAND run_tests)
