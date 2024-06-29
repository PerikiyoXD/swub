cmake_minimum_required(VERSION 3.10)
project(TilingWM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/wm.cpp
    src/background_drawer.cpp
)

# Add executable
add_executable(tiling-wm ${SOURCES})

# Find X11 library
find_package(X11 REQUIRED)

# Link libraries
target_link_libraries(tiling-wm X11)
