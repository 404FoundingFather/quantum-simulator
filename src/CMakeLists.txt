# src/CMakeLists.txt

# Add module subdirectories
add_subdirectory(core)
add_subdirectory(solver)
add_subdirectory(visualization)
add_subdirectory(ui)
add_subdirectory(config)

# Find ImGui library
find_package(imgui CONFIG REQUIRED)

# Main executable
add_executable(quantum_simulator main.cpp)

# Link module libraries and external dependencies
target_link_libraries(quantum_simulator
    PUBLIC core solver visualization ui config
    PRIVATE FFTW3::fftw3 glfw
    PRIVATE imgui::imgui # Link only core imgui
)

# Conditionally add OpenMP if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(quantum_simulator PRIVATE OpenMP::OpenMP_CXX)
endif()

target_include_directories(quantum_simulator PUBLIC ${CMAKE_SOURCE_DIR}/src)
