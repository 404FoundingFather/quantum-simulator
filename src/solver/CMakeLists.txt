# src/solver/CMakeLists.txt
add_library(solver STATIC
    SimulationEngine.cpp
)
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(solver PUBLIC FFTW3::fftw3 OpenMP::OpenMP_CXX)