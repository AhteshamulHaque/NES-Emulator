# Create a library for the PPU module
add_library(ppu STATIC src/ppu.cpp)

# Include headers
target_include_directories(ppu PUBLIC include)

# Add tests
# add_executable(test_ppu test/test_ppu.cpp)
# target_link_libraries(test_ppu ppu)
