cmake_minimum_required(VERSION 3.16)
project(PianoApp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Find required packages
# Try pkg-config first, then fallback to CMake modules
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SDL2 sdl2)
endif()

if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED CONFIG)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/Piano.cpp
    src/InputHandler.cpp
    src/Recorder.cpp
    src/UIRenderer.cpp
    src/AudioEngine.cpp
    src/Application.cpp
)

# Create executable
add_executable(piano ${SOURCES})

# Link libraries
if(SDL2_LIBRARIES)
    target_link_libraries(piano PRIVATE ${SDL2_LIBRARIES} m)
else()
    target_link_libraries(piano PRIVATE SDL2::SDL2 m)
endif()

# Link directories if needed
if(SDL2_LIBRARY_DIRS)
    target_link_directories(piano PRIVATE ${SDL2_LIBRARY_DIRS})
endif()

# Installation targets
install(TARGETS piano DESTINATION bin)
