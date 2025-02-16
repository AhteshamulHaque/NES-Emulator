cmake_minimum_required(VERSION 3.10)
project(nes-emulator)

# Set C++ standard
set(CMAKE_CXX_STANDDARD 17)
set(CMAKE_CXX_STANDDARD_REQUIRED ON)

# Include subdirectories (modules)
add_subdirectory(cpu)
add_subdirectory(ram)
add_subdirectory(ppu)
add_subdirectory(cartridge)
add_subdirectory(controller)
add_subdirectory(emulator)
