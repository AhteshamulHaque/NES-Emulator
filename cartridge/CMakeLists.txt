# Create a library for the Cartridge module
add_library(cartridge STATIC src/cartridge.cpp)

# Include headers
target_include_directories(cartridge PUBLIC include)

# Add tests
# add_executable(test_cartridge test/test_cartridge.cpp)
# target_link_libraries(test_cartridge cartridge)
