cmake_minimum_required(VERSION 3.16)
project(PianoApp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Find required packages
find_package(X11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${X11_INCLUDE_DIR}
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
target_link_libraries(piano PRIVATE
    ${X11_LIBRARIES}
    m
)

# Installation targets
install(TARGETS piano DESTINATION bin)
