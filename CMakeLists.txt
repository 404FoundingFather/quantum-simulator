cmake_minimum_required(VERSION 3.15)
project(QMSim VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set vcpkg-specific variables if using vcpkg
if(DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
  message(STATUS "Using vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
  # Help CMake find packages explicitly when using vcpkg
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(imgui_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/arm64-osx/share/imgui")
    set(glad_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/arm64-osx/share/glad")
    set(FFTW3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/arm64-osx/share/fftw3")
    message(STATUS "Setting imgui_DIR to: ${imgui_DIR}")
    message(STATUS "Setting glad_DIR to: ${glad_DIR}")
    message(STATUS "Setting FFTW3_DIR to: ${FFTW3_DIR}")
  endif()
endif()

# Special handling for OpenMP on macOS with Homebrew
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Check if OpenMP is installed via Homebrew
  find_program(BREW_PROGRAM brew)
  if(BREW_PROGRAM)
    execute_process(
      COMMAND ${BREW_PROGRAM} --prefix libomp
      OUTPUT_VARIABLE HOMEBREW_LIBOMP_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(HOMEBREW_LIBOMP_PREFIX)
      message(STATUS "Found Homebrew libomp at: ${HOMEBREW_LIBOMP_PREFIX}")
      set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
      set(OpenMP_CXX_LIB_NAMES "omp")
      set(OpenMP_omp_LIBRARY ${HOMEBREW_LIBOMP_PREFIX}/lib/libomp.dylib)
      set(OpenMP_CXX_INCLUDE_DIRS ${HOMEBREW_LIBOMP_PREFIX}/include)
    endif()
  endif()
endif()

# External dependencies
find_package(nlohmann_json REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C)
find_package(FFTW3 CONFIG REQUIRED)
find_package(OpenMP)
find_package(glfw3 REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Include subdirectories
add_subdirectory(src)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()