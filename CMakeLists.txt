cmake_minimum_required(VERSION 3.15)
project(QMSim VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External dependencies
find_package(nlohmann_json REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C)
find_package(FFTW3 REQUIRED COMPONENTS dbl)
find_package(OpenMP)
find_package(glfw3 REQUIRED)
find_package(imgui CONFIG REQUIRED) # Use CONFIG mode

# Include subdirectories
add_subdirectory(src)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()