# src/CMakeLists.txt

# Add module subdirectories
add_subdirectory(core)
add_subdirectory(solver)
add_subdirectory(visualization)
add_subdirectory(ui)
add_subdirectory(config)

# Find ImGui library
find_package(imgui CONFIG REQUIRED) # Revert back to CONFIG mode

# Main executable
add_executable(quantum_simulator main.cpp)

# Link module libraries and external dependencies
target_link_libraries(quantum_simulator
    PUBLIC core solver visualization ui config
    PRIVATE FFTW3::fftw3 OpenMP::OpenMP_CXX glfw
    PRIVATE imgui::imgui # Link only core imgui
)

target_include_directories(quantum_simulator PUBLIC ${CMAKE_SOURCE_DIR}/src)
# target_include_directories(quantum_simulator PRIVATE ${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/include) # Remove explicit include dir
