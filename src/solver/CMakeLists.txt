# src/solver/CMakeLists.txt
add_library(solver STATIC
    SimulationEngine.cpp
)
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link to FFTW3
target_link_libraries(solver PUBLIC FFTW3::fftw3)

# Conditionally link to OpenMP only if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(solver PUBLIC OpenMP::OpenMP_CXX)
endif()