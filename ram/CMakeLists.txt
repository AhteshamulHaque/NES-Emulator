# Create a library for the RAM module
add_library(ram STATIC src/ram.cpp)

# Include headers
target_include_directories(ram PUBLIC include)

# Add tests
# add_executable(test_ram test/test_ram.cpp)
# target_link_libraries(test_ram ram)
