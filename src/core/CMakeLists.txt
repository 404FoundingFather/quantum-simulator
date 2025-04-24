# src/core/CMakeLists.txt
add_library(core STATIC
    Potential.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})