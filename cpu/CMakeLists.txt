# Create a library for the CPU module
add_library(cpu STATIC
    src/cpu.cpp
    src/registers.cpp
    src/apu.cpp
)

# Include headers
target_include_directories(cpu PUBLIC include)

# Link dependencies
target_link_libraries(cpu PUBLIC ram)

# Add tests (These are the key changes)
# if(BUILD_TESTS) # Conditional compilation for tests
find_package(Catch2 REQUIRED)

enable_testing()

add_executable(CPUTests
    test/test_LDA.cpp
)

target_link_libraries(CPUTests PRIVATE Catch2::Catch2WithMain cpu)

    # add_test(NAME CPUTest COMMAND CPUTests)
# endif() # End of conditional compilation for tests

# Optional: Install rules (only for the library, not the tests)
# install(TARGETS cpu DESTINATION lib)
# install(FILES include/cpu.h DESTINATION include)
