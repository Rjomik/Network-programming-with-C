cmake_minimum_required(VERSION 3.15)
project("Network programming with C" C)

# Link your three individual projects
add_subdirectory(Windows)
add_subdirectory(Linux)
add_subdirectory(CrossPlatform)