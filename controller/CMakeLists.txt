# Create a library for the Controller module
add_library(controller STATIC src/controller.cpp)

# Include headers
target_include_directories(controller PUBLIC include)

# Add tests
# add_executable(test_controller test/test_controller.cpp)
# target_link_libraries(test_controller controller)
