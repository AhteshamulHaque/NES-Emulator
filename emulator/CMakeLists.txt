# Create the emulator executable
add_executable(emulator src/emulator.cpp)

# Link all libraries
target_link_libraries(
    emulator
    cpu
    ppu
    ram
    cartridge
    controller
)

# Include header from all modules
target_include_directories(emulator PRIVATE
    ${CMAKE_SOURCE_DIR}/cpu/include
    ${CMAKE_SOURCE_DIR}/ppu/include
    ${CMAKE_SOURCE_DIR}/ram/include
    ${CMAKE_SOURCE_DIR}/cartridge/include
    ${CMAKE_SOURCE_DIR}/controller/include
)
